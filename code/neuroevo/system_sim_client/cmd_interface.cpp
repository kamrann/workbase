// cmd_interface.cpp

#include "cmd_interface.h"


cmd_interface::cmd_interface(std::ostream& out):
m_os{ out }
{

}

void cmd_interface::enter_cmd_loop(std::istream& in, std::string const& quit_str)
{
	while(true)
	{
		m_os << prompt() << " ";

		std::string input;
		std::getline(in, input);

		if(input == quit_str)
		{
			break;
		}

		auto pp_result = preprocess_cmd(input);
		if(pp_result)
		{
			// Valid command
			dispatch(std::move(pp_result.fn));
		}
		else
		{
			// Invalid command
			m_os << pp_result.err << std::endl;
		}
	}
}

void cmd_interface::dispatch(cmd_fn fn)
{
	// Default dispatcher just invokes the function synchronously on the interface thread
	fn();
}



