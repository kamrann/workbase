// par_wgt_holder.cpp

#include "par_wgt_holder.h"

#include <Wt/WText>
#include <Wt/WPushButton>
#include <Wt/WVBoxLayout>
#include <Wt/WHBoxLayout>


namespace prm
{
	par_wgt_holder::par_wgt_holder()
	{
		m_stuff_enabled = false;

		m_label = new Wt::WText;
		m_label->setHidden(true);
		m_status = new Wt::WText;
		m_status->setStyleClass("status");
		m_status->setHidden(true);
		m_menu_btn = new Wt::WPushButton;
		m_menu_btn->setStyleClass("dropbutton");
		m_menu_btn->setHidden(true);
	}

	void par_wgt_holder::set_label(std::string text)
	{
		m_label->setText(text);
		m_label->setHidden(text.empty());
	}

	void par_wgt_holder::set_status_text(std::string text)
	{
		m_status->setText(text);
		m_status->setHidden(text.empty());
	}

	void par_wgt_holder::set_menu(Wt::WPopupMenu* menu)
	{
		m_menu_btn->setMenu(menu);
		m_menu_btn->setHidden(menu == nullptr);
		m_stuff_enabled = menu != nullptr;
	}


	boxed_holder::boxed_holder()
	{
		setStyleClass("boxed");

		auto main_vlayout = new Wt::WVBoxLayout;
		main_vlayout->setContentsMargins(0, 0, 0, 0);
		setLayout(main_vlayout);

		auto header_layout = new Wt::WHBoxLayout;
		main_vlayout->addLayout(header_layout, 0, Wt::AlignLeft);

		m_label->setStyleClass("boxedlabel");
		header_layout->addWidget(m_label);
		header_layout->addWidget(m_status);
		header_layout->addWidget(m_menu_btn, 0, Wt::AlignTop);
	}

	void boxed_holder::enable_border(bool enable)
	{
		// TODO: Encompassing label, or only contents??
		if(enable)
		{
			addStyleClass("bordered");
			layout()->setContentsMargins(5, 5, 5, 5);
		}
		else
		{
			removeStyleClass("bordered");
			layout()->setContentsMargins(0, 0, 0, 0);
		}
	}

	void boxed_holder::set_contents(Wt::WWidget* wgt)
	{
		auto vlayout = static_cast<Wt::WVBoxLayout*>(layout());
		vlayout->addWidget(wgt, 0, Wt::AlignLeft | Wt::AlignTop);
	}


	inline_holder::inline_holder()
	{
		auto main_hlayout = new Wt::WHBoxLayout;
		main_hlayout->setContentsMargins(0, 0, 0, 0);
		setLayout(main_hlayout);

		m_label->setStyleClass("inlinelabel");
		main_hlayout->addWidget(m_label, 0, Wt::AlignLeft);
		main_hlayout->addWidget(m_status, 0, Wt::AlignLeft);
		main_hlayout->addWidget(m_menu_btn, 0, Wt::AlignLeft | Wt::AlignTop);
	}

	void inline_holder::enable_border(bool enable)
	{
		if(enable)
		{
			addStyleClass("bordered");
			layout()->setContentsMargins(5, 5, 5, 5);
		}
		else
		{
			removeStyleClass("bordered");
			layout()->setContentsMargins(0, 0, 0, 0);
		}
	}

	void inline_holder::set_contents(Wt::WWidget* wgt)
	{
		auto hlayout = static_cast<Wt::WHBoxLayout*>(layout());
		hlayout->insertWidget(1, wgt, 0, Wt::AlignRight | Wt::AlignTop);
	}
}



