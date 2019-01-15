#pragma once

#include <vector>

#include "packet.h"

std::vector<char> decrypt_header(std::vector<char>::const_iterator begin)
{
	// Creates new vector containing packet header from input buffer.
	std::vector<char> result(begin, begin + sizeof(PacketHeader));

	// Reads memory as PacketHeader structure.
	PacketHeader * packet_header = (PacketHeader*)result.data();
	
	// Decrypts
	packet_header->length ^= 0x1357;
	packet_header->sequence ^= packet_header->length;
	packet_header->options ^= packet_header->length;

	return result;
}