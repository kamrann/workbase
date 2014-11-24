// rectification.h

#ifndef __WB_GA_RECTIFICATION_H
#define __WB_GA_RECTIFICATION_H

#include <functional>


namespace ga {

	class genome;

	// Rectification function form
	typedef std::function< bool(genome&) > rectification_fn_t;

}


#endif


