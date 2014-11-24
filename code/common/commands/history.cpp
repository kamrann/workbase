// history.cpp

#include "history.h"


namespace wb {
	namespace cmd {

		size_t command_history::size() const
		{
			return m_commands.size();
		}
		
		void command_history::record(command c)
		{
			m_commands.push_back(c);
		}
		
		command command_history::at_position(size_t pos) const
		{
			return m_commands[pos];
		}

	}
}




