#pragma once

#include <cstdint>

#include <cryptopp/secblock.h>

namespace Narcissist {
	void ecdsa_to_wif(char *wif, uint8_t prefix, const CryptoPP::SecByteBlock key);
}
