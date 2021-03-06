// interactive_input.h

#ifndef __NE_SYSSIM_INTERACTIVE_INPUT_H
#define __NE_SYSSIM_INTERACTIVE_INPUT_H

#include <set>

#include <string>
#include <vector>


namespace sys {

	typedef std::set< unsigned long > interactive_input_set;

	class interactive_input
	{
	public:
		interactive_input(unsigned long k = 0, bool d = false): key_index(k), down(d)
		{}

	public:
		unsigned long key_index;
		bool down;
	};

}


#endif

