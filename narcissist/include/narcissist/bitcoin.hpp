#pragma once

#include <secp256k1.h>

namespace Narcissist {
	void derive_p2pkh(secp256k1_pubkey *pubkey, char *address,
		size_t *address_length, char network);
}
