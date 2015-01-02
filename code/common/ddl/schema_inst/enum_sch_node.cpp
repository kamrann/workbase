// enum_sch_node.cpp

#include "enum_sch_node.h"


namespace ddl {

	namespace detail {

		enum_sch_node_t::enum_sch_node_t():
			min_sel_{ 0 },
			max_sel_{}
		{

		}

	}

	enum_sch_node::enum_sch_node():
		obj_{ std::make_shared< impl_t >() }
	{

	}

	enum_sch_node::enum_sch_node(detail::enum_sch_node_ptr ptr):
		obj_{ ptr }
	{}

/*	void enum_sch_node::add_option(enum_value_t val)
	{
		obj_->enum_values_.push_back(val);
	}
*/
	void enum_sch_node::enum_values(enum_set_t values)
	{
		obj_->enum_values_ = std::move(values);
	}

	enum_sch_node::enum_set_t enum_sch_node::enum_values() const
	{
		return obj_->enum_values_;
	}

	std::vector< std::string > enum_sch_node::enum_values_str() const
	{
		std::vector< std::string > strings;
		for(auto const& val : obj_->enum_values_)
		{
			strings.push_back(val.str);
		}
		return strings;
	}

	boost::any enum_sch_node::get_data_from_id_string(std::string str) const
	{
		for(auto const& val : obj_->enum_values_)
		{
			if(val.str == str)
			{
				return val.data;
			}
		}
		return boost::any{};
	}
	
	void enum_sch_node::minsel(size_t count)
	{
		obj_->min_sel_ = count;
	}

	size_t enum_sch_node::minsel() const
	{
		return obj_->min_sel_;
	}

	void enum_sch_node::maxsel(boost::optional< size_t > count)
	{
		obj_->max_sel_ = count;
	}

	boost::optional< size_t > enum_sch_node::maxsel() const
	{
		return obj_->max_sel_;
	}

	void enum_sch_node::default(value_t val)
	{
		obj_->default_ = val;
	}

	enum_sch_node::value_t enum_sch_node::default() const
	{
		return obj_->default_;
	}

}




