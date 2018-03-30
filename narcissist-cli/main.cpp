#include <exception>
#include <iostream>
#include <string>
#include <utility>

#include <boost/algorithm/string.hpp>
#include <boost/program_options.hpp>
#include <cryptopp/filters.h>
#include <cryptopp/hex.h>
#include <cryptopp/osrng.h>
#include <cryptopp/secblock.h>
#include <narcissist/bitcoin.hpp>
#include <narcissist/narcissist.hpp>
#include <secp256k1.h>

namespace po = boost::program_options;

using Narcissist::secp256k1ctx;

static CryptoPP::SecByteBlock parse_privkey_hex (const std::string encoded)
{
	CryptoPP::SecByteBlock decoded;
	CryptoPP::HexDecoder decoder;

	decoder.Put((byte* ) encoded.data(), encoded.size());
	decoder.MessageEnd();

	size_t size = decoder.MaxRetrievable();
	if (size && size <= SIZE_MAX)
	{
		decoded.New(size);
		decoder.Get(reinterpret_cast<byte *>(decoded.data()), decoded.size());
	}

	return decoded;
}

static secp256k1_pubkey parse_pubkey_hex(const std::string encoded)
{
	std::string decoded;
	secp256k1_pubkey pubkey;

	CryptoPP::HexDecoder decoder;
	decoder.Put((byte* ) encoded.data(), encoded.size());
	decoder.MessageEnd();
	size_t size = decoder.MaxRetrievable();
	if (size && size <= SIZE_MAX)
	{
		decoded.resize(size);
		decoder.Get(reinterpret_cast<byte *>(&decoded[0]), decoded.size());
	}

	if (!secp256k1_ec_pubkey_parse(secp256k1ctx, &pubkey, reinterpret_cast<const unsigned char*>(decoded.data()), 33))
	{
		throw std::runtime_error("Failed to parse public key");
	}

	return pubkey;
}

int main(int argc, char **argv) {
	Narcissist::setup();

	po::options_description desc("Allowed options");

	desc.add_options()
		("help", "print this message")
		("generate-keypair,G", "generate a keypair for split-key usage")
		("brute-address,B", "address generation mode")
		("combine-split-key,C", "split-key combination mode")
		("brute-address-prefix,p", po::value<std::string>(), "prefix for bruteforcing")
		("split-public-key,P", po::value<std::string>(), "public key, for split-key usage")
		("split-private-key,i", po::value<std::string>(), "private key, for split-key usage")
	;

	po::variables_map vm;
	po::store(po::parse_command_line(argc, argv, desc), vm);
	po::notify(vm);

	if (vm.count("help") || argc == 1)
	{
		std::cout << desc << std::endl;
		goto exit;
	}
	else if (vm.count("generate-keypair"))
	{
		CryptoPP::SecByteBlock secret(32);
		CryptoPP::OS_GenerateRandomBlock(false, secret, secret.size());

		secp256k1_pubkey pubkey;
		if (!secp256k1_ec_pubkey_create(secp256k1ctx, &pubkey,
			secret.data()))
		{
			throw std::runtime_error("Failed to derive public key from private key");
		}

		byte serialized_pubkey[33];
		size_t serialized_pubkey_length = 33;
		secp256k1_ec_pubkey_serialize(secp256k1ctx, serialized_pubkey,
			&serialized_pubkey_length, &pubkey, SECP256K1_EC_COMPRESSED);

		std::string publicHex, privateHex;

		CryptoPP::HexEncoder hex(new CryptoPP::StringSink(publicHex));
		hex.Put(serialized_pubkey, serialized_pubkey_length);
		hex.MessageEnd();
		hex.Detach(new CryptoPP::StringSink(privateHex));
		hex.Put(secret.data(), secret.size());
		hex.MessageEnd();

		std::cout << " Public: " << publicHex << std::endl;
		std::cout << "Private: " << privateHex << std::endl;

		goto exit;
	}
	else if (vm.count("brute-address"))
	{
		std::function<bool(char *)> accept_address;
		std::function<void(const CryptoPP::SecByteBlock, char *)> generate_address;

		if (vm.count("brute-address-prefix"))
		{
			std::string prefix = vm["brute-address-prefix"].as<std::string>();
			accept_address = [prefix](const char *address) -> bool { return boost::algorithm::starts_with(address, prefix); };
		}
		else
		{
			// accept any address
			accept_address = [](const char *address) -> bool { return true; };
		}

		if (vm.count("split-public-key"))
		{
			secp256k1_pubkey pubkey = parse_pubkey_hex(vm["split-public-key"].as<std::string>());
			generate_address = [pubkey](const CryptoPP::SecByteBlock secret, char *address) -> void {
				secp256k1_pubkey tweakedkey = pubkey;

				if (!secp256k1_ec_pubkey_tweak_mul(secp256k1ctx, &tweakedkey, secret.data()))
				{
					// tweak out of range
					throw std::runtime_error("Tweak out of range");
				}

				Narcissist::derive_p2pkh(&tweakedkey, address, nullptr, 0x00);
			};
		}
		else
		{
			generate_address = [](const CryptoPP::SecByteBlock secret, char *address) -> void {
				secp256k1_pubkey pubkey;

				if (!secp256k1_ec_pubkey_create(secp256k1ctx, &pubkey,
					secret.data()))
				{
					throw std::runtime_error("Failed to derive public key from private key");
				}

				Narcissist::derive_p2pkh(&pubkey, address, nullptr, 0x00);
			};
		}

		CryptoPP::SecByteBlock secret(32);
		char address[64];

		for (;;)
		{
			CryptoPP::OS_GenerateRandomBlock(false, secret, secret.size());
			generate_address(secret, address);

			if (accept_address(address))
			{
				break;
			}
		}

		std::string secretHex;
		CryptoPP::HexEncoder hex(new CryptoPP::StringSink(secretHex));
		hex.Put(secret, secret.size());
		hex.MessageEnd();

		std::cout << (vm.count("split-public-key") ? "  Tweak: " : "Private: ") << secretHex << std::endl;
		std::cout << "Address: " << address << std::endl;

		goto exit;
	}
	else if (vm.count("combine-split-key"))
	{
		if (!vm.count("split-public-key") || !vm.count("split-private-key"))
		{
			std::cerr << "Combining a key requires --split-public-key and --split-private-key" << std::endl;
			goto exit;
		}

		CryptoPP::SecByteBlock tweakKey = parse_privkey_hex(vm["split-public-key"].as<std::string>());
		CryptoPP::SecByteBlock privateKey = parse_privkey_hex(vm["split-private-key"].as<std::string>());

		if (!secp256k1_ec_privkey_tweak_mul(secp256k1ctx, privateKey.data(), tweakKey.data()))
		{
			// tweak out of range
			throw std::runtime_error("Tweak out of range");
		}

		secp256k1_pubkey pubkey;

		if (!secp256k1_ec_pubkey_create(secp256k1ctx, &pubkey,
			privateKey.data()))
		{
			throw std::runtime_error("Failed to derive public key from private key");
		}

		byte serialized_pubkey[33];
		size_t serialized_pubkey_length = 33;
		secp256k1_ec_pubkey_serialize(secp256k1ctx, serialized_pubkey,
			&serialized_pubkey_length, &pubkey, SECP256K1_EC_COMPRESSED);

		char address[64] = { 0 };
		Narcissist::derive_p2pkh(&pubkey, address, nullptr, 0x00);

		std::string publicHex, privateHex;

		CryptoPP::HexEncoder hex(new CryptoPP::StringSink(publicHex));
		hex.Put(serialized_pubkey, serialized_pubkey_length);
		hex.MessageEnd();
		hex.Detach(new CryptoPP::StringSink(privateHex));
		hex.Put(privateKey.data(), privateKey.size());
		hex.MessageEnd();

		std::cout << " Public: " << publicHex << std::endl;
		std::cout << "Private: " << privateHex << std::endl;
		std::cout << "Address: " << address << std::endl;
	}

exit:
	Narcissist::destroy();
	return 0;
}
