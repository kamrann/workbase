// cmd_input.cpp

#include "cmd_input.h"


namespace wb {
	namespace cmd {

		command_input::command_input()
		{
			m_history_index = 0;
			m_current_cmd = "";
		}

		void command_input::set_input_callback(input_callback_fn cb)
		{
			m_cb = cb;
		}

		void command_input::set_command_handler(cmd_handler_fn handler)
		{
			m_cmd_handler = handler;
		}
		
		void command_input::set_key_handler(key_handler_fn handler)
		{
			m_key_handler = handler;
		}

		void command_input::dispatch(command c)
		{
			m_history.record(c);

			if(m_cmd_handler)
			{
				m_cmd_handler(c);
			}
		}

		void command_input::process_key(key k)
		{
			// TODO: allow class client to choose bindings

			switch(k.code)
			{
				case key::KeyCode::Up:
				if(m_history_index > 0)
				{
					m_history_index -= 1;
					set_from_history();
				}
				break;

				case key::KeyCode::Down:
				if(m_history_index < m_history.size())
				{
					m_history_index += 1;
					if(m_history_index == m_history.size())
					{
						set_command_input(m_current_cmd);
					}
					else
					{
						set_from_history();
					}
				}
				break;
			}
		}

		void command_input::set_command_input(command c)
		{
			m_cb(c);
		}

		void command_input::set_from_history()
		{
			auto cmd = m_history.at_position(m_history_index);
			set_command_input(cmd);
		}

	}
}



