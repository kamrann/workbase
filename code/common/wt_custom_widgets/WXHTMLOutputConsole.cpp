// WXHTMLOutputConsole.cpp

#include "WXHTMLOutputConsole.h"


WXHTMLOutputConsole::WXHTMLOutputConsole()
{
	setTextFormat(Wt::XHTMLText);

	addStyleClass("console");
}

void WXHTMLOutputConsole::update_display()
{
	setText(get_full_text());
}

std::string WXHTMLOutputConsole::get_full_text() const
{
	std::string txt;
	for(auto const& elem : m_elements)
	{
		txt += elem;
	}
	return txt;
}


