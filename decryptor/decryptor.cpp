#include <iostream>

#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>
#include <boost/filesystem/path.hpp>

#include "processor.h"

int main(int argc, char * argv[])
{
	if (argc != 2)
	{
		std::cerr << "Usage: decryptor <file>" << std::endl;
		return 0;
	}

	boost::filesystem::path input_file = boost::filesystem::absolute(argv[1]);
	if (!boost::filesystem::exists(input_file))
	{
		std::cerr << "Could not find " << input_file << std::endl;
		return 0;
	}

	// Generates output file based on input filename & path.
	std::string input_file_name = input_file.filename().string().c_str();
	int pos = input_file_name.find_last_of('.');
	std::string output_file_name = input_file_name.substr(0, pos) + "_d" + input_file.extension().string().c_str();
	boost::filesystem::path output_file = input_file.parent_path() / output_file_name;

	boost::filesystem::ifstream input_stream(input_file, std::ios::in);
	boost::filesystem::ofstream output_stream(output_file, std::ios::out);

	processor(input_stream, output_stream);

	return 0;
}
