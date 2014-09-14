// pw_exceptions.h

#ifndef __WB_PW_EXCEPTIONS_H
#define __WB_PW_EXCEPTIONS_H

#include <string>
#include <exception>


namespace prm
{
	class required_unspecified: public std::exception
	{
	public:
		required_unspecified(std::string msg): std::exception(msg.c_str())
		{}
	};
}



#endif


