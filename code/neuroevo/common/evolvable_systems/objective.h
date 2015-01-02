// objective.h

#ifndef __WB_EV_SYS_OBJECTIVE_H
#define __WB_EV_SYS_OBJECTIVE_H

#include "genetic_algorithm/objective_value.h"

#include <memory>
#include <functional>


namespace sys {
	namespace ev {

		class objective
		{
		public:
			virtual ga::objective_value::type get_type() = 0;

			virtual void reset() = 0;
			virtual void update() = 0;
			virtual ga::objective_value evaluate() = 0;

		public:
			virtual ~objective() {};
		};

	}
}



#endif

