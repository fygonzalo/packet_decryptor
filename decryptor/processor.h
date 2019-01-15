#pragma once

#include <iostream>

#include <boost/filesystem/fstream.hpp>

#include <nlohmann/json.hpp>

#include "util.h"
#include "host.h"
#include "packet.h"

void processor(std::istream &input, std::ostream &output)
{
	Host host;

	for (std::string line; std::getline(input, line); )
	{
		// Parses network event as json
		nlohmann::json in_network_event = nlohmann::json::parse(line);

		// Creates a char vector parsing string buffer as char.
		std::vector<char> packet_buffer = string_to_hex(in_network_event["buffer"]);

		PacketSource packet_source = PacketSource::Server;
		if (in_network_event["source"] == "Client")
		{
			packet_source = PacketSource::Client;
		}

		std::vector<std::vector<char>> packets;
		try
		{
			// Host mimics a recv event.
			packets = host.recv(packet_source, packet_buffer);
		}
		catch (std::exception &e)
		{
			// There was a problem. Exists execution.
			std::cout << e.what() << std::endl;
			exit(1);
		};

		if (!packets.empty())
		{	
			// There are packets in buffer.

			// Converts packets buffer char vector to human readable hex string.
			std::vector<std::string> packets_str;
			for (int i = 0; i < packets.size(); i++)
			{
				// Creates string buffer from char vector.
				std::string packet_str = hex_to_string(packets[i]);
				packets_str.push_back(packet_str);
			}

			// Creates a json object for output network event.
			auto out_network_event = nlohmann::json{
				{"server", in_network_event["server"]},
				{"timestamp", in_network_event["timestamp"]},
				{"source", in_network_event["source"]},
				{"packets", packets_str}
			};

			// Writes the network event to output stream.
			output << out_network_event << std::endl << std::flush;
		}
	}
}