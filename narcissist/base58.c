/*
 * Copyright 2012-2014 Luke Dashjr
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the standard MIT license.  See COPYING for more details.
 */

#include <stdint.h>
#include <string.h>
#include <sys/types.h>

#include <narcissist/base58.h>

int base58enc(char *b58, size_t *b58sz, const void *data, size_t binsz)
{
	const char b58digits_ordered[] =
		"123456789ABCDEFGHJKLMNPQRSTUVWXYZ"
		"abcdefghijkmnopqrstuvwxyz";
	const uint8_t *bin = data;
	int carry, error = 0;
	ssize_t i, j, high, zcount = 0;
	size_t size;

	while (zcount < binsz && !bin[zcount])
		++zcount;

	size = (binsz - zcount) * 138 / 100 + 1;
	uint8_t buf[size];
	memset(buf, 0, size);

	for (i = zcount, high = size - 1; i < binsz; ++i, high = j) {
		for (carry = bin[i], j = size - 1; (j > high) || carry; --j) {
			carry += 256 * buf[j];
			buf[j] = carry % 58;
			carry /= 58;
		}
	}

	for (j = 0; j < size && !buf[j]; ++j);

	// if (*b58sz <= zcount + size - j) {
	// 	*b58sz = zcount + size - j + 1;
	// 	return -1;
	// }

	if (zcount)
		memset(b58, '1', zcount);
	for (i = zcount; j < size; ++i, ++j)
		b58[i] = b58digits_ordered[buf[j]];
	b58[i] = '\0';
	// *b58sz = i + 1;

	return 0;
}
