// enum_defn_node.cpp

#include "enum_defn_node.h"


namespace ddl {

	namespace detail {

		enum_defn_node_t::enum_defn_node_t():
			min_sel_{ 0 },
			max_sel_{}
		{

		}

	}

	enum_defn_node::enum_defn_node():
		obj_{ std::make_shared< impl_t >() }
	{

	}

	enum_defn_node::enum_defn_node(detail::enum_defn_node_ptr ptr):
		obj_{ ptr }
	{}

	void enum_defn_node::enumvalues_fn(enum_values_fn_t fn)
	{
		obj_->enum_values_fn_ = fn;
	}

	enum_defn_node::enum_set_t enum_defn_node::enum_values() const
	{
		return obj_->enum_values_fn_();
	}

	void enum_defn_node::minsel(size_t count)
	{
		obj_->min_sel_ = count;
	}

	size_t enum_defn_node::minsel() const
	{
		return obj_->min_sel_;
	}

	void enum_defn_node::maxsel(boost::optional< size_t > count)
	{
		obj_->max_sel_ = count;
	}

	boost::optional< size_t > enum_defn_node::maxsel() const
	{
		return obj_->max_sel_;
	}

}




