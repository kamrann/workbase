// WCommandTerminal.h

#ifndef __WB_WTCUSTOM_COMMAND_TERMINAL_H
#define __WB_WTCUSTOM_COMMAND_TERMINAL_H

#include <Wt/WContainerWidget>


class WCommandLine;

template < typename OutputConsole >
class WCommandTerminal:
	public Wt::WContainerWidget
{
public:
	typedef OutputConsole console_t;

public:
	WCommandTerminal();

};


#endif


