// param_list_par.h

#ifndef __NE_RTP_PARAM_LIST_PARAM_H
#define __NE_RTP_PARAM_LIST_PARAM_H

#include "../rtp_param.h"
#include "../rtp_param_widget.h"
#include "../rtp_param_manager.h"

#include <map>
#include <set>


class rtp_paramlist_param_type: public rtp_param_type
{
public:
	virtual boost::any default_value() const
	{
		return boost::any();
	}

	virtual i_param_widget* create_widget(rtp_param_manager* mgr) const
	{
		size_t num_child_params = provide_num_child_params(mgr);
		rtp_param_list_widget* w = new rtp_param_list_widget(this);
		for(size_t i = 0; i < num_child_params; ++i)
		{
			rtp_named_param np = provide_child_param(i, mgr);
			i_param_widget* c = np.type->create_widget(mgr);
			w->add_child(np.name, c);
			if(!np.name.empty())
			{
				mgr->register_widget(np.name, c);
			}
		}

		for(auto const& entry : m_dependencies)
		{
			for(size_t idx : entry.second)
			{
				mgr->register_change_handler(entry.first, std::bind(
					&rtp_paramlist_param_type::recreate_child, this, w, idx, mgr));
			}
		}

		return w;
	}

	virtual rtp_param get_widget_param(i_param_widget const* w) const
	{
		rtp_param_list_widget const* list_w = (rtp_param_list_widget const*)w;
		rtp_param_list p;
		for(size_t i = 0; i < list_w->num_children(); ++i)
		{
			i_param_widget const* c = list_w->get_child(i);
			p.push_back(c->get_param());
		}
		return rtp_param(p);
	}

	virtual size_t provide_num_child_params(rtp_param_manager* mgr) const = 0;
	virtual rtp_named_param provide_child_param(size_t index, rtp_param_manager* mgr) const = 0;
	//virtual rtp_named_param_list provide_param_list(rtp_param_manager* mgr) const = 0;

	void add_dependency(std::string name, size_t dependent_index)
	{
		m_dependencies[name].insert(dependent_index);
	}

	void recreate_child(rtp_param_list_widget* w, size_t index, rtp_param_manager* mgr) const
	{
		rtp_named_param np = provide_child_param(index, mgr);
		i_param_widget* c = np.type->create_widget(mgr);
		w->replace_child(index, np.name, c);
		if(!np.name.empty())
		{
			mgr->register_widget(np.name, c);
		}
	}

protected:
	std::map< std::string, std::set< size_t > > m_dependencies;
};


class rtp_staticparamlist_param_type: public rtp_paramlist_param_type
{
public:
	rtp_staticparamlist_param_type(rtp_named_param_list const& params): m_params(params)
	{}

	virtual size_t provide_num_child_params(rtp_param_manager* mgr) const
	{
		return m_params.size();
	}

	virtual rtp_named_param provide_child_param(size_t index, rtp_param_manager* mgr) const
	{
		return m_params[index];
	}

private:
	rtp_named_param_list m_params;
};


#endif


