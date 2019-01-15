#include "host.h"

#include "crypt_basics.h"
#include "checksum.h"
#include "compression.h"

#define MAX_BUFFER USHRT_MAX

Host::Host()
{
	server_crypt = std::shared_ptr<Crypt>(new CryptCustom());
	client_crypt = std::shared_ptr<Crypt>(new CryptXORIV());
}

std::vector<std::vector<char>> Host::recv(PacketSource source, std::vector<char> buffer)
{
	std::vector<std::vector<char>> result;

	// Pointers to interface components
	std::shared_ptr<Crypt> interface_crypt;
	std::vector<char> * interface_buffer;

	if (source == PacketSource::Server)
	{
		// Sets interface pointers to server components
		interface_buffer = &server_buffer;
		interface_crypt = server_crypt;
	}
	else
	{	
		// Sets interface pointers to client components
		interface_buffer = &client_buffer;
		interface_crypt = client_crypt;
	}

	// Inserts received byte stream into interface buffer.
	interface_buffer->insert(interface_buffer->end(), buffer.cbegin(), buffer.cend());

	if (interface_buffer->size() < sizeof(PacketHeader))
	{
		// There isnt enough bytes to read a packet header. Returns empty vector.
		return result;
	}

	while (interface_buffer->size() >= sizeof(PacketHeader))
	{
		// There is enough bytes to read a packet header.

		// Decrypts packet header from buffer then parses as PacketHeader structure.
		std::vector<char> packet_header_buffer = decrypt_header(interface_buffer->cbegin());
		PacketHeader * packet_header = (PacketHeader *)packet_header_buffer.data();

		int required_bytes = packet_header->length;
		if (packet_header->options & PacketOption::Encrypted)
		{
			// Packet is encrypted so its length isnt the same as especified on packet header.
			// Raw length is multiple of 16.
			required_bytes = 16 * ((required_bytes + 15) / 16);
		}

		if (required_bytes <= 0)
		{
			// Packet is empty
			throw std::exception("Packet size cant be zero.");
		}
		else if (required_bytes > MAX_BUFFER)
		{
			// Packet requires more bytes that buffer capacity.
			throw std::exception("Packet size is bigger than expected");
		}
		else if (required_bytes + sizeof(PacketHeader) > interface_buffer->size())
		{
			// Didnt received enough bytes. Wait for next byte streams.
			break;
		}
		else
		{
			// There is enough bytes to process a packet.
			// Copies its content to a new buffer.
			std::vector<char> packet_buffer(packet_header_buffer);
			packet_buffer.insert(packet_buffer.end(), interface_buffer->cbegin() + sizeof(PacketHeader), interface_buffer->cbegin() + sizeof(PacketHeader) + packet_header->length);

			// Auxiliar pointer to the start of payload on packet buffer.
			auto pb_payload_begin = packet_buffer.begin() + sizeof(PacketHeader);

			if (packet_header->options & PacketOption::Encrypted)
			{
				// Packet is encrypted. Calculates encrypted buffer length.
				int eb_len = 16 * ((packet_header->length + 15) / 16);

				// Sets pointers to encrypted buffer.
				auto eb_begin = interface_buffer->cbegin() + sizeof(PacketHeader);
				auto eb_end = eb_begin + eb_len;

				// Decrypts buffer using the interface crypt service.
				std::vector<char> decrypted_buffer = interface_crypt->decrypt(eb_begin, eb_end);

				// Sets pointers to decrypted buffer.
				auto db_begin = decrypted_buffer.cbegin();
				auto db_end = db_begin + packet_header->length;

				// Copies decrypted buffer to packet buffer.
				std::copy(db_begin, db_end, pb_payload_begin);
			}
			
			char calculated_checksum = checksum(packet_buffer.cbegin() + sizeof(PacketHeader), packet_buffer.cend());
			if (packet_header->checksum != calculated_checksum)
			{
				// Checksum failed.
				throw std::exception("Integrity checksum failed. Could not continue.");

				// TODO implement custom exception to return this packet.
			}

			if (packet_header->options & PacketOption::Compressed)
			{
				// Packet is compressed.

				// Sets pointers to compressed buffer.
				auto cb_begin = packet_buffer.cbegin() + sizeof(PacketHeader);
				auto cb_end = packet_buffer.cend();

				// Decompress buffer.
				std::vector<char> decompressed_buffer = decompress(cb_begin, cb_end);

				// Resizes packet buffer.
				int db_size = sizeof(PacketHeader) + decompressed_buffer.size();
				packet_buffer.resize(db_size);

				// Sets pointers to decompressed buffer.
				auto db_begin = decompressed_buffer.cbegin();
				auto db_end = decompressed_buffer.cend();

				// Resets pointers to packet payload because of reallocation.
				pb_payload_begin = packet_buffer.begin() + sizeof(PacketHeader);

				// Copies decompressed buffer to packet buffer.
				std::copy(db_begin, db_end, pb_payload_begin);
			}

			// Removes processed bytes from interface buffer.
			int processed_bytes = sizeof(PacketHeader) + required_bytes;
			interface_buffer->erase(interface_buffer->begin(), interface_buffer->begin() + processed_bytes);

			// Push processed packet into result vector.
			result.push_back(packet_buffer);

			if (source == PacketSource::Server)
			{
				// Packet received from server
				if (packet_header->sequence == PACKET_HANDSHAKE)
				{
					// Packet sequence indicates this packet is for handshake

					// Sets pointer to packet payload end.
					auto pb_payload_end = packet_buffer.end();

					handshake(pb_payload_begin, pb_payload_end);
				}
			}
		}
	}

	// Returns vector with packets.
	return result;
}

void Host::handshake(std::vector<char>::const_iterator begin, std::vector<char>::const_iterator end)
{
	// Gets a pointer to client key.
	auto client_key = begin + sizeof(short);

	// Sets client key.
	int client_key_len = *(int *)&*client_key;
	auto client_key_begin = client_key + sizeof(client_key_len);
	client_crypt->set_key(client_key_begin, client_key_begin + client_key_len);

	if (std::shared_ptr<CryptCustom> server_custom_crypt = std::static_pointer_cast<CryptCustom>(server_crypt))
	{
		// Server is using custom crypt service.

		// Sets server algorithm.
		auto server_algorithm = client_key_begin + client_key_len;
		int server_algorithm_len = *(int *)&*server_algorithm;
		auto server_algorithm_begin = server_algorithm + sizeof(server_algorithm_len);
		server_custom_crypt->set_algorithm(server_algorithm_begin, server_algorithm_begin + server_algorithm_len);

		// Sets server key.
		auto server_key = server_algorithm_begin + server_algorithm_len;
		int server_key_len = *(int *)&*server_key;
		auto server_key_begin = server_key + sizeof(server_key_len);
		server_custom_crypt->set_key(server_key_begin, server_key_begin + server_key_len);
	}
	else
	{
		// Server is using defined crypt service.
		server_crypt->set_key(client_key_begin, client_key_begin + client_key_len);
	}
}