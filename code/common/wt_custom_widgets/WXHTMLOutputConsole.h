// WXHTMLOutputConsole.h

#ifndef __WB_WTCUSTOM_XHTML_OUTPUT_CONSOLE_H
#define __WB_WTCUSTOM_XHTML_OUTPUT_CONSOLE_H

#include "WOutputConsoleBase.h"

#include <Wt/WText>


class WXHTMLOutputConsole:
	public Wt::WText,
	public WOutputConsoleBase
{
public:
	WXHTMLOutputConsole();

protected:
	virtual void update_display() override;

protected:
	std::string get_full_text() const;
};


#endif


