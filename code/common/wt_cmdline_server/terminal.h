// terminal.h

#ifndef __WB_WTDISP_TERMINAL
#define __WB_WTDISP_TERMINAL

#include "display_types.h"
#include "display_base.h"

#include <Wt/WContainerWidget>


class WCommandLine;
class WOutputConsoleBase;
class WPlainOutputConsole;

class terminal_display:
	public Wt::WContainerWidget
{
public:
	terminal_display(/**/);

public:
	WCommandLine* get_cmdline() const;
	WOutputConsoleBase* get_console() const;

private:
	WCommandLine* m_cmdline;
	WPlainOutputConsole* m_console;
};



#endif


