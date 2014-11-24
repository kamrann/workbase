// cmd_input.h

#ifndef __WB_COMMAND_INPUT_H
#define __WB_COMMAND_INPUT_H

#include "cmd.h"
#include "key.h"
#include "history.h"

#include <functional>


namespace wb {
	namespace cmd {

		class command_input
		{
		public:
			typedef std::function< void(command) > input_callback_fn;
			typedef std::function< void(command) > cmd_handler_fn;
			typedef std::function< void(key) > key_handler_fn;

		public:
			command_input();

			void set_input_callback(input_callback_fn cb);
			void set_command_handler(cmd_handler_fn handler);
			void set_key_handler(key_handler_fn handler);

			void dispatch(command c);
			void process_key(key k);

		protected:
			void set_command_input(command c);
			void set_from_history();

		protected:
			input_callback_fn m_cb;
			cmd_handler_fn m_cmd_handler;
			key_handler_fn m_key_handler;
			command_history m_history;
			size_t m_history_index;
			command m_current_cmd;
		};

	}
}


#endif


