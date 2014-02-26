// fixed_or_random_par.cpp

#include "fixed_or_random_par.h"
#include "realnum_par.h"
#include "../rtp_param_widget.h"
#include "../rtp_defs.h"

#include <Wt/WComboBox>
#include <Wt/WText>

#include <boost/random/uniform_real_distribution.hpp>


i_param_widget* rtp_fixed_or_random_param_type::create_widget(rtp_param_manager* mgr) const
{
	rtp_param_widget< Wt::WContainerWidget >* widget = new rtp_param_widget< Wt::WContainerWidget >(this);
	Wt::WComboBox* box = new Wt::WComboBox(widget);
	box->addItem("Fixed");
	box->addItem("Random");

	rtp_realnum_param_type* fixed_pt = new rtp_realnum_param_type(m_default, m_min, m_max);
	//fixed_pt->use_spin(true, )
	i_param_widget* fixed = fixed_pt->create_widget(mgr);
	widget->addWidget(fixed->get_wt_widget());
	widget->addWidget(new Wt::WText("["));
	i_param_widget* rand_min = (new rtp_realnum_param_type(m_default_min, m_min, m_max))->create_widget(mgr);
	widget->addWidget(rand_min->get_wt_widget());
	i_param_widget* rand_max = (new rtp_realnum_param_type(m_default_max, m_min, m_max))->create_widget(mgr);
	widget->addWidget(rand_max->get_wt_widget());
	widget->addWidget(new Wt::WText(")"));

	box->changed().connect(std::bind([=](){
		int index = box->currentIndex();
		if(index == 0)
		{
/*			fixed->get_wt_widget()->setHidden(false);
			rand_min->get_wt_widget()->setHidden(true);
			rand_max->get_wt_widget()->setHidden(true);
*/
			widget->widget(1)->setHidden(false);
			for(size_t i = 2; i < widget->count(); ++i)
			{
				widget->widget(i)->setHidden(true);
			}
		}
		else
		{
/*			fixed->get_wt_widget()->setHidden(true);
			rand_min->get_wt_widget()->setHidden(false);
			rand_max->get_wt_widget()->setHidden(false);
*/
			widget->widget(1)->setHidden(true);
			for(size_t i = 2; i < widget->count(); ++i)
			{
				widget->widget(i)->setHidden(false);
			}
		}
	}));
	box->changed().emit();
	
	return widget;
}

rtp_param rtp_fixed_or_random_param_type::get_widget_param(i_param_widget const* w) const
{
	Wt::WContainerWidget const* cont = (Wt::WContainerWidget const*)w->get_wt_widget();
	Wt::WComboBox const* box = (Wt::WComboBox const*)cont->widget(0);
	bool is_fixed = box->currentIndex() == 0;
	if(is_fixed)
	{
		i_param_widget const* fixed = dynamic_cast<i_param_widget const*>(cont->widget(1));
		return rtp_param(fixed_or_random< double, boost::random::uniform_real_distribution< double >, rgen_t >(
			boost::any_cast<double>(fixed->get_param())));
	}
	else
	{
		i_param_widget const* rand_min = dynamic_cast<i_param_widget const*>(cont->widget(3));
		i_param_widget const* rand_max = dynamic_cast<i_param_widget const*>(cont->widget(4));
		return rtp_param(fixed_or_random< double, boost::random::uniform_real_distribution< double >, rgen_t >(
			boost::any_cast<double>(rand_min->get_param()),
			boost::any_cast<double>(rand_max->get_param())));
	}
}


