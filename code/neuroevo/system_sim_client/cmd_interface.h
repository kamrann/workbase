// cmd_interface.h

#ifndef __WB_CMD_INTERFACE_H
#define __WB_CMD_INTERFACE_H

#include <istream>
#include <ostream>
#include <string>
#include <functional>


class cmd_interface
{
public:
	typedef std::function< void() > cmd_fn;

	struct preprocessed_cmd
	{
		preprocessed_cmd()
		{}

		preprocessed_cmd(cmd_fn _fn): fn{ _fn }
		{}

		preprocessed_cmd(std::string _err): err{ _err }
		{}

		operator bool() const
		{
			return (bool)fn;
		}

		cmd_fn fn;
		std::string err;
	};

public:
	cmd_interface(std::ostream& out);

public:
	void enter_cmd_loop(std::istream& in, std::string const& quit_str = ":q");

private:
	virtual std::string prompt() const = 0;
	virtual preprocessed_cmd preprocess_cmd(std::string cmd_str) = 0;

	virtual void dispatch(cmd_fn fn);

private:
	std::ostream& m_os;
};



#endif


