// WCommandLine.cpp

#include "WCommandLine.h"

#include <Wt/WHBoxLayout>
#include <Wt/WText>
#include <Wt/WLineEdit>


WCommandLine::WCommandLine()
{
	auto layout_ = new Wt::WHBoxLayout{};
	setLayout(layout_);

	m_prompt = new Wt::WText{};
	m_prompt->setText(">");
	layout_->addWidget(m_prompt);

	m_command_edit = new Wt::WLineEdit{};
	m_command_edit->enterPressed().connect(std::bind([this]
	{
		auto cmd = m_command_edit->text().toUTF8();
		m_command_edit->setText("");
		m_cmd_signal.emit();
	}));
	layout_->addWidget(m_command_edit);
}

void WCommandLine::set_prompt(std::string prompt)
{
	m_prompt->setText(prompt);
}

Wt::Signal< std::string >& WCommandLine::on_command()
{
	return m_cmd_signal;
}





