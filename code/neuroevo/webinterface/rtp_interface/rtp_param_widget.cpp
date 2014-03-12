// rtp_param_widget.cpp

#include "rtp_param_widget.h"
#include "params/paramlist_par.h"
#include "rtp_indent_widget.h"

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

rtp_param_list_widget::~rtp_param_list_widget()
{

}

void rtp_param_list_widget::add_child(std::string name, i_param_widget* c)
{
	int existing_rows = children.size();

	if(name.empty())
	{
		rtp_indent_widget* iw = new rtp_indent_widget();
		iw->set_child(c);
		m_layout->addWidget(iw, existing_rows, 0, 1, 2);// , Wt::AlignLeft);
	}
	else
	{
		m_layout->addWidget(new Wt::WText(name), existing_rows, 0);// , 1, 2);
		m_layout->addWidget(c->get_wt_widget(), existing_rows, 1, 1, 1, Wt::AlignLeft);
	}

	m_layout->setColumnStretch(0, 0);
	m_layout->setColumnStretch(1, 1);
	
	children.push_back(c);
}

void rtp_param_list_widget::replace_child(size_t index, std::string name, i_param_widget* c)//, rtp_param_manager* mgr)
{
	// TODO: This seems like assumptions we probably shouldn't be making about the layout...
	size_t const col_count = 2;
	size_t left_index = index * col_count;
	size_t right_index = left_index + 1;

	Wt::WLayoutItem* item = m_layout->itemAt(right_index);
	if(item != nullptr)
	{
		// Existing entry at this location is labelled
		Wt::WWidget* label = m_layout->itemAt(left_index)->widget();
		m_layout->removeWidget(label);
		delete label;

		Wt::WWidget* old = item->widget();
		m_layout->removeWidget(old);
		delete old;
	}
	else
	{
		// Unlabelled
		Wt::WWidget* old = m_layout->itemAt(left_index)->widget();
		m_layout->removeWidget(old);
		delete old;
	}

	//m_layout->removeWidget(children[index]->get_wt_widget());
	
	if(name.empty())
	{
		rtp_indent_widget* iw = new rtp_indent_widget();
		iw->set_child(c);
		m_layout->addWidget(iw, index, 0, 1, 2);// , Wt::AlignLeft);
	}
	else
	{
		Wt::WText* label = new Wt::WText(name);
		m_layout->addWidget(label, index, 0);
		m_layout->addWidget(c->get_wt_widget(), index, 1, 1, 1, Wt::AlignLeft);
	}
	
	children[index] = c;
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
	Wt::WWidget* old = widget(0);
	removeWidget(old);
	// See below
	delete old;
	//
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
	Wt::WWidget* old = widget(1);
	removeWidget(old);
	// TODO: Assuming we are given ownership of each widget pass in.
	// Probably better to use shared_ptr to pass and store the selection and nested widgets
	delete old;
	//
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


