// WCommandLine.h

#ifndef __WB_WTCUSTOM_COMMAND_LINE_H
#define __WB_WTCUSTOM_COMMAND_LINE_H

#include <Wt/WContainerWidget>

#include <string>


namespace Wt {
	class WLabel;
	class WLineEdit;
}


class WCommandLine:
	public Wt::WContainerWidget
{
public:
	WCommandLine();

public:
	void set_prompt(std::string prompt);
	void set_command(std::string cmd);

	Wt::Signal< std::string >& on_command();
	Wt::Signal< Wt::Key >& on_key();

private:
	Wt::WLabel* m_prompt;
	Wt::WLineEdit* m_command_edit;

	Wt::Signal< std::string > m_cmd_signal;
	Wt::Signal< Wt::Key > m_key_signal;
};


#endif


