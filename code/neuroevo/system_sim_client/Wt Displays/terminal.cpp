// terminal.cpp

#include "terminal.h"

#include "wt_custom_widgets/WPlainOutputConsole.h"
#include "wt_custom_widgets/WCommandLine.h"

#include <Wt/WVBoxLayout>
#include <Wt/WScrollArea>


terminal_display::terminal_display(/**/)
{
	auto _layout = new Wt::WVBoxLayout;
	_layout->setContentsMargins(5, 5, 5, 5);
	setLayout(_layout);

//	auto scroll = new Wt::WScrollArea{};
	m_console = new WPlainOutputConsole{};
//	scroll->setWidget(m_console);
//	_layout->addWidget(scroll, 1);
	_layout->addWidget(m_console, 1);

	m_cmdline = new WCommandLine{};
	m_cmdline->setTabIndex(0);
	_layout->addWidget(m_cmdline);

	m_console->clicked().connect(std::bind([this]
	{
		m_cmdline->setFirstFocus();
	}));

	m_cmdline->setFirstFocus();

	addStyleClass("terminal");
}

WCommandLine* terminal_display::get_cmdline() const
{
	return m_cmdline;
}

WOutputConsoleBase* terminal_display::get_console() const
{
	return m_console;
}





