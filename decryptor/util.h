#pragma once

#include <vector>

constexpr char hex_map[] = { '0', '1', '2', '3', '4', '5', '6', '7',
						   '8', '9', 'A', 'B', 'C', 'D', 'E', 'F' };

std::vector<char> string_to_hex(std::string buffer)
{
	int len = buffer.length();

	// Reserves a char vector. Size is half of length because each pair of hex
	// digits (4 bits) represents one character (8 bits)
	std::vector<char> result(len / 2);

	for (int i = 0; i < len; i+=2)
	{
		// Extracts value of each hex digit.
		char lo = buffer[i + 1] <= '9' ? buffer[i + 1] - '0' : buffer[i + 1] - 'A' + 10;
		char hi = buffer[i] <= '9' ? buffer[i] - '0' : buffer[i] - 'A' + 10;

		// Parses as character
		char byte = hi << 4 | lo;

		result[i / 2] = byte;
	}

	return result;
}

std::string hex_to_string(std::vector<char> buffer)
{
	int len = buffer.size();

	// Reserves a string. Length must be double of buffer size because two
	// ascii digits (4 bits) are required per character (8 bits).
	std::string result(len * 2, ' ');

	// Iterates over all characters
	for (int i = 0; i < len; ++i) {
		// Extracts hi and lo from current char then maps to its hex representation.
		// Faster than casting each char into an hex string stream
		result[2 * i] = hex_map[((unsigned char)buffer[i] & 0xF0) >> 4];
		result[2 * i + 1] = hex_map[((unsigned char)buffer[i] & 0x0F)];
	}

	return result;
}