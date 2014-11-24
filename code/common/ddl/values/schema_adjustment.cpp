// schema_adjustment.cpp

#include "value_node.h"


namespace ddl {

	struct adjust_visitor
	{
		typedef void result_type;

		adjust_visitor(value_node& val):
			val_{ val }
		{}

		inline void operator() (boolean_sch_node const& nd) //const
		{
			if(!val_.is_bool() || !fits_schema(val_, nd))
			{
				val_ = nd.default();
			}
		}

		inline void operator() (integer_sch_node const& nd) //const
		{
			if(!val_.is_int() || !fits_schema(val_, nd))
			{
				val_ = nd.default();
			}
		}

		inline void operator() (realnum_sch_node const& nd) //const
		{
			if(!val_.is_real() || !fits_schema(val_, nd))
			{
				val_ = nd.default();
			}
		}

		inline void operator() (string_sch_node const& nd) //const
		{
			if(!val_.is_string() || !fits_schema(val_, nd))
			{
				val_ = nd.default();
			}
		}

		inline void operator() (enum_sch_node const& nd) //const
		{
			if(!val_.is_enum() || !fits_schema(val_, nd))
			{
				val_ = nd.default();
			}
		}

		inline void operator() (list_sch_node const& nd) //const
		{
/*			if(!val_.is_list() || !fits_schema(val_, nd))
			{
				val_ = std::vector < value_node > {};
			}
*/
			// Default is empty list
			std::vector< value_node > items;

			if(val_.is_list())
			{
				auto existing = val_.as_list();
				auto item_schemas = nd.entries();

				if(existing.size() != item_schemas.size())
				{
					// TODO: Suspect this should never happen?
					throw std::runtime_error("list schema item count mismatch");
				}

				// Iterate over existing items
				for(size_t i = 0; i < existing.size(); ++i)
				{
					auto item_val = existing[i];
					auto const& item_sch = item_schemas[i];

					// Recursively adjust value for item based on corresponding schema
					adjust_to_schema(item_val, item_sch);

					items.push_back(item_val);
				}
			}

			val_ = items;
		}

		inline void operator() (composite_sch_node const& nd) //const
		{
			std::map< node_name, value_node > components;
			// If already composite, start off with existing child values
			if(val_.is_composite())
			{
				components = val_.as_composite();

				// TODO: May not want to do this...?
				// Remove any children which do not exist in current schema
				for(auto it = std::begin(components); it != std::end(components); )
				{
					if(nd.has_child(it->first))
					{
						++it;
					}
					else
					{
						it = components.erase(it);
					}
				}
			}
			
			// Iterate over schema children
			auto sch_children = nd.children();
			for(auto const& c : sch_children)
			{
				auto id = c.first;
				auto sn = c.second;
				// If value for this child does not exist, default construct one
				if(components.find(id) == components.end())
				{
					components[id] = value_node{};
				}

				// Recursively adjust value for child based on child schema
				adjust_to_schema(components[id], sn);
			}

			val_ = components;
		}

		value_node& val_;
	};

	void adjust_to_schema(value_node& vals, sch_node const& sch)
	{
		vals.set_id(sch.get_id());

		auto vis = adjust_visitor{ vals };
		sch.apply_visitor(vis);
	}

}



