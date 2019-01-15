#pragma once

#define PACKET_HANDSHAKE -1

enum PacketSource {
	Server,
	Client
};

enum PacketOption {
	Encrypted = 0x01,
	Compressed = 0x80
};

struct PacketHeader {
	unsigned short length;
	short sequence;
	char options;
	char checksum;
};