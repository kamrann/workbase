// WPlainOutputConsole.cpp

#include "WPlainOutputConsole.h"


WPlainOutputConsole::WPlainOutputConsole()
{
	setTextFormat(Wt::PlainText);
}

void WPlainOutputConsole::update_display()
{
	setText(get_full_text());
}

std::string WPlainOutputConsole::get_full_text() const
{
	std::string txt;
	for(auto const& elem : m_elements)
	{
		txt += elem;
		txt += "\n";
	}
	return txt;
}


