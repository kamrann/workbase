// history.h

#ifndef __WB_COMMAND_HISTORY_H
#define __WB_COMMAND_HISTORY_H

#include "cmd.h"

#include <vector>


namespace wb {
	namespace cmd {

		class command_history
		{
		public:
			size_t size() const;
			void record(command c);
			command at_position(size_t pos) const;

		protected:
			std::vector< command > m_commands;
		};

	}
}


#endif


