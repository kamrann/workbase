// value_objective_defn.cpp

#include "value_objective.h"

//#include "params/schema_builder.h"
//#include "params/param_accessor.h"


namespace sys {
	namespace ev {

		value_objective_defn::value_objective_defn(state_val_access_fn_t& sv_acc_fn):
			sv_acc_fn_(sv_acc_fn)
		{
			
		}

		using namespace ddl;

		defn_node value_objective_defn::get_defn(specifier& spc)
		{
			auto dir_enum_vals = ddl::define_enum_fixed{};
			for(auto entry : value_objective::DirectionTypeMp.left)
			{
				dir_enum_vals = dir_enum_vals(entry.second);
			}
			ddl::defn_node direction_type = spc.enumeration("direction_type")
				(ddl::spc_range < size_t > { 1, 1 })
				(dir_enum_vals)
				;

			auto get_sv_names_fn = ddl::enum_defn_node::enum_values_str_fn_t{ [this](ddl::navigator nav)
			{
				auto sv_ids = sv_acc_fn_(nav);

				auto val_names = ddl::enum_defn_node::enum_str_set_t{};
				std::transform(
					std::begin(sv_ids),
					std::end(sv_ids),
					std::inserter(val_names, std::end(val_names)),
					[](state_value_id const& svid)
				{
					return svid.to_string();
				});
				return val_names;
			}
			};
			get_sv_names_fn.add_dependency(sv_acc_fn_);

			ddl::defn_node state_val = spc.enumeration("state_val")
				(ddl::spc_range < size_t >((size_t)0, 1))
				(define_enum_func{ get_sv_names_fn })
				;

			auto valtype_enum_vals = ddl::define_enum_fixed{};
			for(auto entry : value_objective::ValueTypeMp.left)
			{
				valtype_enum_vals = valtype_enum_vals(entry.second);
			}
			ddl::defn_node value_type = spc.enumeration("value_type")
				(ddl::spc_range < size_t > { 1, 1 })
				(valtype_enum_vals)
				;

			// TODO: just one enum defn node with conditional values function?
			defn_node tp_type = spc.enumeration("timepoint_type")
				(ddl::spc_range < size_t > { 1, 1 })
				(define_enum_fixed{}
				(value_objective::TimePointTypeMp.at(value_objective::TimePointType::SimulationEnd))
				(value_objective::TimePointTypeMp.at(value_objective::TimePointType::WhenFirst))
				(value_objective::TimePointTypeMp.at(value_objective::TimePointType::WhenLast))
				);

			defn_node from_tp_type = spc.enumeration("from_timepoint_type")
				(ddl::spc_range < size_t > { 1, 1 })
				(define_enum_fixed{}
				(value_objective::TimePointTypeMp.at(value_objective::TimePointType::SimulationStart))
				(value_objective::TimePointTypeMp.at(value_objective::TimePointType::WhenFirst))
				(value_objective::TimePointTypeMp.at(value_objective::TimePointType::WhenLast))
				);

			defn_node until_tp_type = spc.enumeration("until_timepoint_type")
				(ddl::spc_range < size_t > { 1, 1 })
				(define_enum_fixed{}
				(value_objective::TimePointTypeMp.at(value_objective::TimePointType::SimulationEnd))
				(value_objective::TimePointTypeMp.at(value_objective::TimePointType::WhenFirst))
				(value_objective::TimePointTypeMp.at(value_objective::TimePointType::WhenLast))
				);

			defn_node extended_timespan = spc.composite("extended_timespan")(define_children{}
				("from", from_tp_type)
				("until", until_tp_type)
				);

			auto rf_value_type = node_ref{ value_type };
			defn_node timespan = spc.conditional("timespan")
				(spc_condition{ cnd::equal{ rf_value_type, value_objective::ValueTypeMp.left.at(value_objective::ValueType::ValueAt) }, tp_type })
				(spc_default_condition{ extended_timespan })
				;

			return spc.composite("val_obj")(ddl::define_children{}
				("direction_type", direction_type)
				("state_val", state_val)
				("value_type", value_type)
				("timespan", timespan)
				);
		}

		std::unique_ptr< value_objective > value_objective_defn::generate(
			ddl::navigator nav,
			std::function< double(state_value_id) > get_state_value_fn
			)
		{
			std::unique_ptr< value_objective > obj;

			auto dir_type = value_objective::DirectionTypeMp.at(
				nav["direction_type"].get().as_single_enum_str());

			auto state_val_id = state_value_id::from_string(
				nav["state_val"].get().as_single_enum_str());

			auto value_type = value_objective::ValueTypeMp.at(
				nav["value_type"].get().as_single_enum_str());

			nav = nav["timespan"][(size_t)0];

			switch(value_type)
			{
				case value_objective::ValueType::ValueAt:
				{
					auto tp_type = value_objective::TimePointTypeMp.at(
						nav.get().as_single_enum_str());
					obj = std::make_unique< single_timepoint_objective >(dir_type, state_val_id, value_type, tp_type);
				}
				break;

				default:
				{
					auto from_tp_type = value_objective::TimePointTypeMp.at(
						nav["from"].get().as_single_enum_str());
					auto until_tp_type = value_objective::TimePointTypeMp.at(
						nav["until"].get().as_single_enum_str());
					obj = std::make_unique< timespan_objective >(dir_type, state_val_id, value_type, from_tp_type, until_tp_type);
				}
				break;
			}

			obj->set_stateval_fn(get_state_value_fn);
			return obj;
		}

	}
}


#if 0

namespace YAML {

	template <>
	struct convert < rtp::value_objective::DirectionType >
	{
		static Node encode(rtp::value_objective::DirectionType const& rhs)
		{
			return Node(rtp::value_objective::DirectionTypeMp.at(rhs));
		}

		static bool decode(Node const& node, rtp::value_objective::DirectionType& rhs)
		{
			if(!node.IsScalar())
			{
				return false;
			}

			auto it = mapping_.find(node.Scalar());
			if(it == mapping_.end())
			{
				return false;
			}

			rhs = it->second;
			return true;
		}

		static std::map< std::string, rtp::value_objective::DirectionType > const mapping_;
	};

	std::map< std::string, rtp::value_objective::DirectionType > const convert< rtp::value_objective::DirectionType >::mapping_ = {
			{ "Minimise", rtp::value_objective::DirectionType::Minimise },
			{ "Maximise", rtp::value_objective::DirectionType::Maximise },
	};

	template <>
	struct convert < rtp::value_objective::ValueType >
	{
		static Node encode(rtp::value_objective::ValueType const& rhs)
		{
			return Node(rtp::value_objective::ValueTypeMp.at(rhs));
		}

		static bool decode(Node const& node, rtp::value_objective::ValueType& rhs)
		{
			if(!node.IsScalar())
			{
				return false;
			}

			auto it = mapping_.find(node.Scalar());
			if(it == mapping_.end())
			{
				return false;
			}

			rhs = it->second;
			return true;
		}

		static std::map< std::string, rtp::value_objective::ValueType > const mapping_;
	};

	std::map< std::string, rtp::value_objective::ValueType > const convert< rtp::value_objective::ValueType >::mapping_ = {
			{ "Value", rtp::value_objective::ValueType::ValueAt },
			{ "Average", rtp::value_objective::ValueType::Average },
			{ "Sum", rtp::value_objective::ValueType::Sum },
			{ "Minimum", rtp::value_objective::ValueType::Minimum },
			{ "Maximum", rtp::value_objective::ValueType::Maximum },
	};

	template <>
	struct convert < rtp::value_objective::TimePointType >
	{
		static Node encode(rtp::value_objective::TimePointType const& rhs)
		{
			return Node(rtp::value_objective::TimePointTypeMp.at(rhs));
		}

		static bool decode(Node const& node, rtp::value_objective::TimePointType& rhs)
		{
			if(!node.IsScalar())
			{
				return false;
			}

			auto it = mapping_.find(node.Scalar());
			if(it == mapping_.end())
			{
				return false;
			}

			rhs = it->second;
			return true;
		}

		static std::map< std::string, rtp::value_objective::TimePointType > const mapping_;
	};

	std::map< std::string, rtp::value_objective::TimePointType > const convert< rtp::value_objective::TimePointType >::mapping_ = {
			{ "Start", rtp::value_objective::TimePointType::SimulationStart },
			{ "End", rtp::value_objective::TimePointType::SimulationEnd },
			{ "When First", rtp::value_objective::TimePointType::WhenFirst },
			{ "When Last", rtp::value_objective::TimePointType::WhenLast },
	};
}

#endif


