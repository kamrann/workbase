// connection_access.h

#ifndef __WB_NN_CONNECTION_ACCESS_H
#define __WB_NN_CONNECTION_ACCESS_H

#include "iterator.h"


namespace nnet {

	class i_connection_access
	{
	public:
		virtual size_t count() const = 0;

		virtual connection_iterator begin() = 0;
		virtual connection_iterator end() = 0;

	public:
		virtual ~i_connection_access()
		{}
	};

}


#endif


