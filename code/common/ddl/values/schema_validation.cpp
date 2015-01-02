// schema_validation.cpp

#include "value_node.h"


namespace ddl {

	// NOTE: Assumes that value and schema nodes are of matching type
	struct check_visitor
	{
		typedef bool result_type;

		check_visitor(value_node const& val):
			val_{ val }
		{}

		inline bool operator() (boolean_sch_node const& nd) const
		{
			auto value = val_.as_bool();
			// No constraints to check
			return true;
		}

		inline bool operator() (integer_sch_node const& nd) const
		{
			auto value = val_.as_int();
			auto min = nd.min();
			if(min && value < min)
			{
				return false;
			}
			auto max = nd.max();
			if(max && value > max)
			{
				return false;
			}
			return true;
		}

		inline bool operator() (realnum_sch_node const& nd) const
		{
			auto value = val_.as_real();
			auto min = nd.min();
			if(min && value < min)
			{
				return false;
			}
			auto max = nd.max();
			if(max && value > max)
			{
				return false;
			}
			return true;
		}

		inline bool operator() (string_sch_node const& nd) const
		{
			auto value = val_.as_string();
			auto str_len = value.length();
			auto minlen = nd.minlength();
			if(str_len < minlen)
			{
				return false;
			}
			auto max = nd.maxlength();
			if(max && str_len > max)
			{
				return false;
			}
			// TODO: nd.charset
			return true;
		}

		inline bool operator() (enum_sch_node const& nd) const
		{
			auto value = val_.as_enum_str();
			auto sel_count = value.size();
			auto minsel = nd.minsel();
			if(sel_count < minsel)
			{
				// TODO: need to enforce a default which conforms to these restrictions, before we
				// can enforce them here (ie. if min>0, ensure that something is selected on creation.
				//return false;
			}
			auto maxsel = nd.maxsel();
			if(maxsel && sel_count > maxsel)
			{
				return false;
			}
			auto options = nd.enum_values_str();
			for(auto const& ev : value)
			{
				if(std::find(std::begin(options), std::end(options), ev) == options.end())
				{
					return false;
				}
			}
			return true;
		}

		inline bool operator() (list_sch_node const& nd) const
		{
			// TODO: May want to change behaviour of this - currently very harsh, entire list has to fit.
			// Perhaps deal with this similarly to how composite is done?
/*			auto value = val_.as_list();
			auto item_schemas = nd.entries();
			if(value.size() != item_schemas.size())
			{
				return false;
			}
			for(size_t i = 0; i < value.size(); ++i)
			{
				auto const& item_val = value[i];
				auto const& item_sch = item_schemas[i];

				if(!fits_schema(item_val, item_sch))
				{
					return false;
				}
			}
*/			return true;
		}

		inline bool operator() (composite_sch_node const& nd) const
		{
			// TODO: Need to do anything?
			return true;
		}

		inline bool operator() (conditional_sch_node const& nd) const
		{
			// TODO: Need to do anything?
			return true;
		}

		value_node const& val_;
	};

	bool fits_schema(value_node const& vals, sch_node const& sch)
	{
		auto vis = check_visitor{ vals };
		return sch.apply_visitor(vis);
	}

}



