#pragma once

#include <secp256k1.h>

namespace Narcissist {
	extern secp256k1_context *secp256k1ctx;

	void setup();

	void destroy();
}
