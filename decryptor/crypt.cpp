#include "crypt.h"

#include <memory.h>
#include <Windows.h>

#define CUSTOM_KEY_SIZE 0x84


void CryptXOR::set_key(std::vector<char>::const_iterator begin, std::vector<char>::const_iterator end)
{
	key = std::vector<char>(begin, end);
}

std::vector<char> CryptXOR::decrypt(std::vector<char>::const_iterator begin, std::vector<char>::const_iterator end)
{
	int size = end - begin;

	std::vector<char> result(size);

	int src_dwords = size / sizeof(int);
	int key_dwords = key.size() / sizeof(int);

	int * p_key = (int *)key.data();
	int * p_dst = (int *)result.data();
	int * p_src = (int *)&*begin;

	for (int i = 0; i < src_dwords; i++)
	{
		*p_dst++ = *(int *)(p_key + (i & (key_dwords - 1))) ^ *p_src++;
	}

	return result;
}

std::vector<char> CryptXORIV::decrypt(std::vector<char>::const_iterator begin, std::vector<char>::const_iterator end)
{
	std::vector<char> result = CryptXOR::decrypt(begin, end);

	int size = end - begin;

	int key_dwords = key.size() / sizeof(int);

	int * p_key = (int *)key.data();

	for (int i = 0; i < key_dwords; i++)
	{
		*p_key++ += size;
	}

	return result;
}

CryptCustom::~CryptCustom()
{
	delete_key();
}

void CryptCustom::delete_key()
{
	if (!key.empty())
	{
		int * p_key = (int *)key.data();

		int number_segments = *p_key++;
		
		for (int i = 0; i < number_segments; i++)
		{
			char * p_segment = *(char**)p_key;
			delete[] p_segment; *p_key++ = 0;
		}
	}
}

void CryptCustom::set_key(std::vector<char>::const_iterator begin, std::vector<char>::const_iterator end)
{
	delete_key();

	key = std::vector<char>(CUSTOM_KEY_SIZE);

	int * p_src = (int *)&*begin;
	int * p_key = (int *)key.data();

	int number_segments = *p_src++;

	*p_key++ = number_segments;

	for (int i = 0; i < number_segments; i++)
	{
		int segment_len = *p_src++;

		char * segment_data = new char[segment_len];
		memcpy(segment_data, (void *)p_src, segment_len);
		p_src = (int *)((char *)p_src + segment_len);

		memcpy((void *)p_key, &segment_data, sizeof(segment_data));
		*(p_key++ + 17) = segment_len;
	}
}

void CryptCustom::set_algorithm(std::vector<char>::const_iterator begin, std::vector<char>::const_iterator end)
{
	int size = end - begin;
	const void * p_src = (const void *)&*begin;

	decryption_algorithm = VirtualAlloc(NULL, size, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
	if (decryption_algorithm)
	{
		memcpy(decryption_algorithm, p_src, size);
	}
}

std::vector<char> CryptCustom::decrypt(std::vector<char>::const_iterator begin, std::vector<char>::const_iterator end)
{
	int size = end - begin;

	std::vector<char> result(size);

	void * p_src = (void *)&*begin;
	void * p_dst = (void *)result.data();
	void * p_key = (void *)(key.data() + 4);

	(reinterpret_cast<int(__cdecl*)(void*, void*, unsigned int, void *)>(decryption_algorithm))(p_src, p_dst, (unsigned int)(size), p_key);

	return result;
}