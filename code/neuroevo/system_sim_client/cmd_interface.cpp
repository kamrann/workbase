// cmd_interface.cpp

#include "cmd_interface.h"


cmd_interface::cmd_interface(std::ostream& out):
m_os{ out }
{

}

cmd_interface::Result cmd_interface::process(std::string cmd_str)
{
/*	if(cmd_str == quit_str)
	{

	}
	*/

	auto pp_result = preprocess_cmd(cmd_str);
	if(pp_result)
	{
		// Valid command
		dispatch(std::move(pp_result.fn));
		return Result::Success;
	}
	else
	{
		// Invalid command
		return Result::ParseFailure;
	}
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

		auto res = process(input);
		switch(res)
		{
			case Result::ParseFailure:
			m_os << "parse failure" << std::endl;// TODO: pp_result.err << std::endl;
			break;
		}
	}
}

void cmd_interface::dispatch(cmd_fn fn)
{
	// Default dispatcher just invokes the function synchronously on the interface thread
	fn();
}



