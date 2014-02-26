// rtp_param_widget.cpp

#include "rtp_param_widget.h"
#include "params/paramlist_par.h"

#include <Wt/WGridLayout>
#include <Wt/WText>


rtp_param_list_widget::rtp_param_list_widget(rtp_paramlist_param_type const* _type): base_t(_type)
{
	m_layout = new Wt::WGridLayout();
	m_layout->setContentsMargins(0, 0, 0, 0);
	m_layout->setHorizontalSpacing(5);
	m_layout->setVerticalSpacing(5);
	setLayout(m_layout);
}

void rtp_param_list_widget::add_child(std::string name, i_param_widget* c)
{
	int existing_rows = children.size();

	m_layout->addWidget(new Wt::WText(name), existing_rows, 0);
	m_layout->addWidget(c->get_wt_widget(), existing_rows, 1);// , Wt::AlignLeft);

	m_layout->setColumnStretch(0, 0);
	m_layout->setColumnStretch(1, 1);
	
	children.push_back(c);
}

size_t rtp_param_list_widget::num_children() const
{
	return children.size();
}

i_param_widget* rtp_param_list_widget::get_child(size_t idx)
{
	return children[idx];
}

i_param_widget const* rtp_param_list_widget::get_child(size_t idx) const
{
	return children[idx];
}


rtp_nested_param_widget::rtp_nested_param_widget(rtp_param_type const* _type): base_t(_type)
{
	selection_widget = nullptr;
	nested_widget = nullptr;
	// Placeholders
	addWidget(new Wt::WContainerWidget);
	addWidget(new Wt::WContainerWidget);
}

void rtp_nested_param_widget::set_selection_widget(i_param_widget* c)
{
	selection_widget = c;
	removeWidget(widget(0));
	insertWidget(0, c->get_wt_widget());
}

i_param_widget* rtp_nested_param_widget::get_selection_widget()
{
	return selection_widget;
}

i_param_widget const* rtp_nested_param_widget::get_selection_widget() const
{
	return selection_widget;
}

void rtp_nested_param_widget::set_nested_widget(i_param_widget* c)
{
	nested_widget = c;
	removeWidget(widget(1));
	insertWidget(1, c->get_wt_widget());
}

i_param_widget* rtp_nested_param_widget::get_nested_widget()
{
	return nested_widget;
}

i_param_widget const* rtp_nested_param_widget::get_nested_widget() const
{
	return nested_widget;
}


