// enum_defn_node.cpp

#include "enum_defn_node.h"
#include "../values/navigator.h"


namespace ddl {

	enum_defn_node::enum_defn_node():
		enum_values_fn_{},
		min_sel_{ 0 },
		max_sel_{},
		default_{}
	{

	}

	void enum_defn_node::enumvalues_fn(enum_values_fn_t fn)
	{
		enum_values_fn_ = fn;
	}

	enum_defn_node::enum_set_t enum_defn_node::enum_values(navigator nav) const
	{
		return enum_values_fn_(nav);
	}

	void enum_defn_node::minsel(size_t count)
	{
		min_sel_ = count;
	}

	size_t enum_defn_node::minsel() const
	{
		return min_sel_;
	}

	void enum_defn_node::maxsel(boost::optional< size_t > count)
	{
		max_sel_ = count;
	}

	boost::optional< size_t > enum_defn_node::maxsel() const
	{
		return max_sel_;
	}

	void enum_defn_node::default(value_t val)
	{
		default_ = val;
	}

	enum_defn_node::value_t enum_defn_node::default() const
	{
		return default_;
	}

	dep_list enum_defn_node::deps() const
	{
		return enum_values_fn_.deps();
	}

}




