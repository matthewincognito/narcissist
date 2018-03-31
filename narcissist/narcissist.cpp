#include <cstdint>
#include <exception>

#include <cryptopp/osrng.h>
#include <secp256k1.h>

#include <narcissist/narcissist.hpp>

secp256k1_context *Narcissist::secp256k1ctx;

void Narcissist::setup()
{
	secp256k1ctx =
		secp256k1_context_create(SECP256K1_CONTEXT_SIGN | SECP256K1_CONTEXT_VERIFY);

	uint8_t seed[32];
	CryptoPP::OS_GenerateRandomBlock(false, seed, 32);
	if (!secp256k1_context_randomize(secp256k1ctx, seed))
	{
		throw std::runtime_error("Failed to randomize secp256k1 context");
	}
}

void Narcissist::destroy()
{
	if (secp256k1ctx)
	{
		secp256k1_context_destroy(secp256k1ctx);
		secp256k1ctx = nullptr;
	}
}
