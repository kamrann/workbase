// WCommandLine.cpp

#include "WCommandLine.h"

#include <Wt/WHBoxLayout>
//#include <Wt/WText>
#include <Wt/WLabel>
#include <Wt/WLineEdit>


WCommandLine::WCommandLine()
{
	auto layout_ = new Wt::WHBoxLayout{};
	layout_->setContentsMargins(0, 0, 0, 0);
	layout_->setSpacing(0);
	setLayout(layout_);

	m_prompt = new Wt::WLabel{};
	m_prompt->setText(">");
	m_prompt->addStyleClass("prompt");
	layout_->addWidget(m_prompt, 0, Wt::AlignBottom);

	m_command_edit = new Wt::WLineEdit{};
	m_command_edit->setAttributeValue("spellcheck", "false");
	m_command_edit->addStyleClass("cmd_entry");
	m_command_edit->enterPressed().connect(std::bind([this]
	{
		auto cmd = m_command_edit->text().toUTF8();
		m_command_edit->setText("");
		m_cmd_signal.emit(cmd);
	}));
	m_command_edit->keyWentDown().connect(std::bind([this](Wt::WKeyEvent ev)
	{
		m_key_signal.emit(ev.key());
	}, std::placeholders::_1));

//	m_command_edit->setMinimumSize(200.0, Wt::WLength::Auto);
//	m_command_edit->setWidth(Wt::WLength::Auto);
	layout_->addWidget(m_command_edit, 1, Wt::AlignBottom);

	m_prompt->setBuddy(m_command_edit);

	addStyleClass("cmdline");
}

void WCommandLine::set_prompt(std::string prompt)
{
	m_prompt->setText(prompt);
}

void WCommandLine::set_command(std::string cmd)
{
	m_command_edit->setText(cmd);
}

Wt::Signal< std::string >& WCommandLine::on_command()
{
	return m_cmd_signal;
}

Wt::Signal< Wt::Key >& WCommandLine::on_key()
{
	return m_key_signal;
}





