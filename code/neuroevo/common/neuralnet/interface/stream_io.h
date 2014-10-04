// stream_io.h

#ifndef __WB_NN_STREAM_IO_H
#define __WB_NN_STREAM_IO_H

#include "nnet_fwd.h"

#include <ostream>


namespace nnet {

	std::ostream& operator<< (std::ostream& strm, input& in);
	std::ostream& operator<< (std::ostream& strm, input_range& in);
	std::ostream& operator<< (std::ostream& strm, output& in);

}


#endif


