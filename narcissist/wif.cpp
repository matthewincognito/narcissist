#include <cstdint>

#include <cryptopp/secblock.h>
#include <cryptopp/sha.h>

#include <narcissist/base58.h>
#include <narcissist/wif.hpp>

static CryptoPP::SHA256 sha256;

static inline void sha256d(uint8_t *out, const uint8_t *in, size_t in_length)
{
	// sha256(sha256(x))

	sha256.CalculateDigest(out, in, in_length);
	sha256.CalculateDigest(out, out, CryptoPP::SHA256::DIGESTSIZE);
}

void Narcissist::ecdsa_to_wif(char *base58, uint8_t prefix, const CryptoPP::SecByteBlock key)
{
	// 1 byte prefix + key + 0x01 + 4 bytes of checksum
	CryptoPP::SecByteBlock wif(key.size() + 6);

	wif.data()[0] = prefix;
	memcpy(wif.data() + 1, key, key.size());
	wif.data()[key.size() + 1] = 0x01;

	uint8_t checksum[CryptoPP::SHA256::DIGESTSIZE];
	sha256d(checksum, wif, key.size() + 2);
	memcpy(wif.data() + key.size() + 2, checksum, 4);

	base58enc(base58, nullptr, wif.data(), wif.size());
}
