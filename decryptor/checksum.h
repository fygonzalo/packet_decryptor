#pragma once

#include <vector>

#define CHECKSUM_IV 0xD31F

char checksum(std::vector<char>::const_iterator begin, std::vector<char>::const_iterator end)
{
	unsigned short result = CHECKSUM_IV;

	int size = end - begin;

	for (int i = 0; i < (size / 2) && begin < end; i++)
	{;
		result ^= *(const unsigned short *)(&*begin);
		begin += sizeof(short);
	}

	unsigned int number_bitshifts = result % 16;

	result = (result << number_bitshifts) | (result >> (16 - number_bitshifts));
	return result ^ (result >> 8);
}