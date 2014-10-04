// text_io.h

#ifndef __TEXT_IO_H
#define __TEXT_IO_H

#include <fstream>
#include <streambuf>
#include <string>
#include <istream>


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

template < typename Char >
struct eat_manip
{
	typedef Char char_t;
	typedef std::basic_string< Char > string_t;
	typedef std::basic_istream< Char > stream_t;

	eat_manip(string_t const& _str): str(_str)
	{}

	inline stream_t& operator() (stream_t& strm) const
	{
		for(auto const& ch : str)
		{
			char_t read;
			strm >> read;
			if(read != ch)
			{
				strm.setstate(std::ios::failbit);
				break;
			}
		}

		return strm;
	}

	friend inline stream_t& operator>> (stream_t& strm, eat_manip& m)
	{
		return m(strm);
	}

	string_t str;
};

typedef eat_manip< char > eat;


#endif

