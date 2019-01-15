#pragma once

#include <vector>

#include <lzo/lzo1x.h>

std::vector<char> decompress(std::vector<char>::const_iterator src_begin, std::vector<char>::const_iterator src_end)
{
	// Initializes result vector with max expected capacity and size.
	std::vector<char> result(USHRT_MAX);

	int src_len = src_end - src_begin;

	unsigned long dst_len = 0;
	lzo1x_decompress((const unsigned char *)&*src_begin, src_len, (unsigned char *)result.data(), &dst_len, NULL);

	// Resizes to decompressed size.
	result.resize(dst_len);

	return result;
}