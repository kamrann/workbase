// text_io.h

#ifndef __TEXT_IO_H
#define __TEXT_IO_H

#include <fstream>
#include <streambuf>
#include <string>


inline std::string read_file_to_string(std::string const& filename)
{
	std::ifstream file(filename);
	if(!file.is_open())
	{
		return "";
	}

	return std::string(
		(std::istreambuf_iterator<char>(file)),
		std::istreambuf_iterator<char>());
}


#endif

