#pragma once

#include <vector>

class Crypt {
public:
	virtual void set_key(std::vector<char>::const_iterator begin, std::vector<char>::const_iterator end) = 0;
	virtual std::vector<char> decrypt(std::vector<char>::const_iterator begin, std::vector<char>::const_iterator end) = 0;
protected:
	std::vector<char> key;
};

class CryptXOR : public Crypt {
public:
	virtual void set_key(std::vector<char>::const_iterator begin, std::vector<char>::const_iterator end);
	virtual std::vector<char> decrypt(std::vector<char>::const_iterator begin, std::vector<char>::const_iterator end);
};

class CryptXORIV : public CryptXOR {
public:
	virtual std::vector<char> decrypt(std::vector<char>::const_iterator begin, std::vector<char>::const_iterator end);
};

class CryptCustom : public Crypt {
public:
	~CryptCustom();
	virtual void set_key(std::vector<char>::const_iterator begin, std::vector<char>::const_iterator end);
	virtual void set_algorithm(std::vector<char>::const_iterator begin, std::vector<char>::const_iterator end);
	virtual std::vector<char> decrypt(std::vector<char>::const_iterator begin, std::vector<char>::const_iterator end);
private:
	void * decryption_algorithm;

	void delete_key();
};