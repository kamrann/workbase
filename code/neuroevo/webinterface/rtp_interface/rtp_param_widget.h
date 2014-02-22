// rtp_param_widget.h

#ifndef __NE_RTP_PARAM_WIDGET_H
#define __NE_RTP_PARAM_WIDGET_H

#include "rtp_param.h"

#include <Wt/WContainerWidget>


class i_param_widget
{
public:
	virtual Wt::WWidget* get_wt_widget() = 0;
	virtual Wt::WWidget const* get_wt_widget() const = 0;
	virtual rtp_param_type const* get_param_type() const = 0;
	virtual rtp_param get_param() const = 0;
};

template < typename WtWidget >
class rtp_param_widget:
	public i_param_widget,
	public WtWidget	// Must derive from WWidget
{
public:
	rtp_param_widget(rtp_param_type const* _type): type(_type)
	{}

public:
	virtual Wt::WWidget* get_wt_widget()
	{
		return (WtWidget*)this;
	}

	virtual Wt::WWidget const* get_wt_widget() const
	{
		return (WtWidget const*)this;
	}

	virtual rtp_param_type const* get_param_type() const
	{
		return type.get();
	}

	virtual rtp_param get_param() const
	{
		return type->get_widget_param(this);
	}

private:
	boost::shared_ptr< rtp_param_type const > type;
};


class rtp_paramlist_param_type;
namespace Wt {
	class WGridLayout;
}

class rtp_param_list_widget: public rtp_param_widget< Wt::WContainerWidget >
{
public:
	typedef rtp_param_widget< Wt::WContainerWidget > base_t;

public:
	rtp_param_list_widget(rtp_paramlist_param_type const* _type);

public:
	void add_child(std::string name, i_param_widget* c);
	size_t num_children() const;
	i_param_widget* get_child(size_t idx);
	i_param_widget const* get_child(size_t idx) const;

private:
	Wt::WGridLayout* m_layout;
	std::vector< i_param_widget* > children;
};

class rtp_nested_param_widget: public rtp_param_widget< Wt::WContainerWidget >
{
public:
	typedef rtp_param_widget< Wt::WContainerWidget > base_t;

public:
	rtp_nested_param_widget(rtp_param_type const* _type);

public:
	void set_selection_widget(i_param_widget* c);
	i_param_widget* get_selection_widget();
	i_param_widget const* get_selection_widget() const;
	void set_nested_widget(i_param_widget* c);
	i_param_widget* get_nested_widget();
	i_param_widget const* get_nested_widget() const;

private:
	i_param_widget* selection_widget;
	i_param_widget* nested_widget;
};


#endif


