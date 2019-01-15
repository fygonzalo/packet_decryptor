#pragma once

#include <vector>
#include <memory>

#include "packet.h"
#include "crypt.h"

class Host {
public:
	Host();
	std::vector<std::vector<char>> recv(PacketSource source, std::vector<char> buf);
private:
	std::vector<char> client_buffer;
	std::vector<char> server_buffer;

	std::shared_ptr<Crypt> server_crypt;
	std::shared_ptr<Crypt> client_crypt;

	void handshake(std::vector<char>::const_iterator begin, std::vector<char>::const_iterator end);
};