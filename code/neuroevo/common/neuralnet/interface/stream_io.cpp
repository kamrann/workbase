// stream_io.cpp

#include "stream_io.h"

#include "input.h"
#include "output.h"

#include "util/interval_io.h"


namespace nnet {

	std::ostream& operator<< (std::ostream& strm, input& in)
	{
		strm << "[ ";
		for(auto val : in)
		{
			strm << val << " ";
		}
		strm << "]";
		return strm;
	}

	std::ostream& operator<< (std::ostream& strm, input_range& in)
	{
		strm << "[ ";
		for(auto rg : in)
		{
			strm << rg << " ";
		}
		strm << "]";
		return strm;
	}

	std::ostream& operator<< (std::ostream& strm, output& out)
	{
		strm << "[ ";
		for(auto val : out)
		{
			strm << val << " ";
		}
		strm << "]";
		return strm;
	}

#if 0
	std::istream& operator>>(std::istream& is, T& obj)
	{
		// read obj from stream
		if( /* T could not be constructed */ )
			is.setstate(std::ios::failbit);
		return is;
	}
#endif
}

