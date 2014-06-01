// container_par_wgt.cpp

#include "container_par_wgt.h"

#include <Wt/WContainerWidget>

#include <boost/variant/get.hpp>


namespace prm
{
	// TODO: Multiple implementations (eg. combo box based), which can be selected through options of param_wgt::create()
	class container_par_wgt::impl: public Wt::WContainerWidget
	{

	};

	Wt::WWidget* container_par_wgt::create_impl(pw_options const& opt)
	{
		m_impl = new impl();
		return m_impl;
	}

	void container_par_wgt::add_child(param_wgt* w)
	{
		m_impl->addWidget(w);
	}

	void container_par_wgt::clear_children(size_t from_index)
	{
		while(m_impl->count() > from_index)
		{
			m_impl->removeWidget(m_impl->widget(from_index));
		}
	}

	param container_par_wgt::get_param() const
	{
		std::vector< param > child_params;
		auto num = m_impl->count();
		for(auto i = 0; i < num; ++i)
		{
			Wt::WWidget* w = m_impl->widget(i);
			// TODO: For now just assuming that all children are param_wgts
			param_wgt* pw = (param_wgt*)w;
			child_params.push_back(pw->get_param());
		}
		return child_params;
	}

	void container_par_wgt::set_from_param(param const& p)
	{
		//m_impl->setText(boost::get< std::string >(p));
		// TODO: To implement. Clear children then use visitor to actually create new widgets???
	}
}



