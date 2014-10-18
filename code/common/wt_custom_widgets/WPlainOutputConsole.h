// WPlainOutputConsole.h

#ifndef __WB_WTCUSTOM_PLAIN_OUTPUT_CONSOLE_H
#define __WB_WTCUSTOM_PLAIN_OUTPUT_CONSOLE_H

#include "WOutputConsoleBase.h"

#include <Wt/WText>


class WPlainOutputConsole:
	public Wt::WText,
	public WOutputConsoleBase
{
public:
	WPlainOutputConsole();

protected:
	virtual void update_display() override;

protected:
	std::string get_full_text() const;
};


#endif


