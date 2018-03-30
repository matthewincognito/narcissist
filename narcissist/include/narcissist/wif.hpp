#pragma once

#include <cryptopp/secblock.h>

namespace Narcissist {
	void ecdsa_to_wif(char *wif, byte prefix, const CryptoPP::SecByteBlock key);
}
