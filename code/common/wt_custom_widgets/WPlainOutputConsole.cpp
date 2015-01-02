// WPlainOutputConsole.cpp

#include "WPlainOutputConsole.h"

#include <Wt/WApplication>
#include <Wt/WStandardItem>


WPlainOutputConsole::WPlainOutputConsole()
{
	m_model = new Wt::WStandardItemModel{};
	m_model->appendColumn(std::vector < Wt::WStandardItem* > { new Wt::WStandardItem{ "I hate Wt" } });
		//Wt::WStringListModel{};
	setModel(m_model);

	setHeaderHeight(0);
	setRowHeight(14);
	setSelectionMode(Wt::SelectionMode::NoSelection);
	setEditTriggers(Wt::WAbstractItemView::EditTrigger::NoEditTrigger);
	setLayoutSizeAware(true);

	addStyleClass("console");
}

void WPlainOutputConsole::add_element(std::string elem)
{
	// TODO: temp hack for clearing console 
	if(elem.empty())
	{
		clear();
		return;
	}

	// This seems to help a bit, but there is still some double rendering going on with the scroll bar
//	Wt::WApplication::instance()->deferRendering();
//	auto row = m_model->rowCount();
	auto html = std::string{};
	auto it = std::begin(elem);
	while(it != std::end(elem) && *it == ' ')
	{
		html += "&nbsp;";
		++it;
	}
	html.insert(std::end(html), it, std::end(elem));
//	m_model->addString(tmp);// elem);
	auto item = new Wt::WStandardItem{ html };
	item->setFlags(Wt::ItemFlag::ItemIsXHTMLText);
	m_model->appendRow(item);
//	m_model->setFlags(row, Wt::ItemFlag::ItemIsXHTMLText);
	scrollTo(m_model->index(m_model->rowCount() - 1, 0), Wt::WTableView::ScrollHint::PositionAtBottom);
//	update_display();
//	Wt::WApplication::instance()->resumeRendering();

	m_elements.push_back(std::move(elem));
}

void WPlainOutputConsole::clear()
{
	m_model->removeRows(0, m_model->rowCount());
	m_elements.clear();
}

void WPlainOutputConsole::update_display()
{

	/*
	// This seems to help a bit, but there is still some double rendering going on with the scroll bar
	Wt::WApplication::instance()->deferRendering();

	// TODO: doing complete reset of model - inefficient
	m_model->removeRows(0, m_model->rowCount());

//	m_text->setText(get_full_text());
	for(auto const& e : m_elements)
	{
		m_model->addString(e);
	}

	scrollTo(m_model->index(m_model->rowCount() - 1, 0), Wt::WTableView::ScrollHint::PositionAtBottom);

	Wt::WApplication::instance()->resumeRendering();
	*/
}

void WPlainOutputConsole::layoutSizeChanged(int width, int height)
{
	auto const ColumnWidthAdjust = 7;	// See WAbstractItemView docs
	bool const VScrollVisible = true;	// TODO: How to detect if scroll bar is showing?
	auto const VScrollAdjust = VScrollVisible ? 17 : 0;	// TODO: Detect scroll bar width?
	
	setColumnWidth(0, std::max(width - ColumnWidthAdjust - VScrollAdjust, 0));
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


