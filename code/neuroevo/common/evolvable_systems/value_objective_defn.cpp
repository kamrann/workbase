// value_objective_defn.cpp

#include "value_objective.h"

#include "params/schema_builder.h"
#include "params/param_accessor.h"


namespace sys {
	namespace ev {

		value_objective_defn::value_objective_defn(state_val_access_fn_t sv_acc_fn):
			sv_acc_fn_(sv_acc_fn)
		{
			
		}

		namespace sb = prm::schema;

		void value_objective_defn::update_schema_provider(prm::schema::schema_provider_map_handle provider, prm::qualified_path const& prefix)
		{
			auto path = prefix;

			(*provider)[path + std::string("direction_type")] = [](prm::param_accessor)
			{
				auto dir_type_name_list = std::vector < std::string > {};
				std::transform(
					std::begin(value_objective::DirectionTypeMp.left),
					std::end(value_objective::DirectionTypeMp.left),
					std::back_inserter(dir_type_name_list),
					[](value_objective::direction_type_map_t::left_value_type const& val)
				{
					return val.second;
				});
				auto s = sb::enum_selection(
					"direction_type",
					dir_type_name_list
					);
				sb::unlabel(s);
				return s;
			};

			(*provider)[path + std::string("state_val")] = [this](prm::param_accessor acc)
			{
				auto sv_ids = sv_acc_fn_(acc);

				auto val_names = std::vector < std::string > {};
				std::transform(
					std::begin(sv_ids),
					std::end(sv_ids),
					std::back_inserter(val_names),
					[](state_value_id const& svid)
				{
					return svid.to_string();
				});

				auto s = sb::enum_selection(
					"state_val",
					val_names,
					0, 1
					);
//				sb::unlabel(s);
//				sb::update_on(s, "evolved_controller_compatibility_changed");
				return s;
			};

			(*provider)[path + std::string("value_type")] = [](prm::param_accessor)
			{
				auto value_type_name_list = std::vector < std::string > {};
				std::transform(
					std::begin(value_objective::ValueTypeMp.left),
					std::end(value_objective::ValueTypeMp.left),
					std::back_inserter(value_type_name_list),
					[](value_objective::value_type_map_t::left_value_type const& val)
				{
					return val.second;
				});
				auto s = sb::enum_selection(
					"value_type",
					value_type_name_list
					);
//				sb::unlabel(s);
//				sb::trigger(s, "obj_value_type_changed");
				return s;
			};


			path += std::string("objective_timespan");

			(*provider)[path + std::string("timepoint_type")] = [](prm::param_accessor)
			{
				auto s = sb::enum_selection(
					"timepoint_type",
					{
					value_objective::TimePointTypeMp.at(value_objective::TimePointType::SimulationEnd),
					value_objective::TimePointTypeMp.at(value_objective::TimePointType::WhenFirst),
					value_objective::TimePointTypeMp.at(value_objective::TimePointType::WhenLast),
					}
				);
				sb::label(s, "at");
				return s;
			};

			(*provider)[path + std::string("from_timepoint_type")] = [](prm::param_accessor)
			{
				auto s = sb::enum_selection(
					"from_timepoint_type",
					{
					value_objective::TimePointTypeMp.at(value_objective::TimePointType::SimulationStart),
					value_objective::TimePointTypeMp.at(value_objective::TimePointType::WhenFirst),
					value_objective::TimePointTypeMp.at(value_objective::TimePointType::WhenLast),
					}
				);
				sb::label(s, "between");
				return s;
			};

			(*provider)[path + std::string("until_timepoint_type")] = [](prm::param_accessor)
			{
				auto s = sb::enum_selection(
					"until_timepoint_type",
					{
					value_objective::TimePointTypeMp.at(value_objective::TimePointType::SimulationEnd),
					value_objective::TimePointTypeMp.at(value_objective::TimePointType::WhenFirst),
					value_objective::TimePointTypeMp.at(value_objective::TimePointType::WhenLast),
					}
				);
				sb::label(s, "and");
				return s;
			};

			(*provider)[path] = [=](prm::param_accessor acc)
			{
				auto s = sb::list("objective_timespan");

				auto value_type = value_objective::DefaultValueType;
				if(acc.is_available("value_type"))
				{
					auto value_type_str = prm::extract_as< prm::enum_param_val >(acc["value_type"])[0];
					value_type = value_objective::ValueTypeMp.at(value_type_str);
				}
				switch(value_type)
				{
					case value_objective::ValueType::ValueAt:
					sb::append(s, provider->at(path + std::string("timepoint_type"))(acc));
					break;

					default:
					sb::append(s, provider->at(path + std::string("from_timepoint_type"))(acc));
					sb::append(s, provider->at(path + std::string("until_timepoint_type"))(acc));
					break;
				}

				//sb::update_on(s, "obj_value_type_changed");
				//sb::unborder(s);
				//sb::layout_horizontal(s);
				return s;
			};

			path.pop();


			(*provider)[path] = [=](prm::param_accessor acc)
			{
				auto s = sb::list(path.leaf().name());
				sb::append(s, provider->at(path + std::string("direction_type"))(acc));
				sb::append(s, provider->at(path + std::string("state_val"))(acc));
				sb::append(s, provider->at(path + std::string("value_type"))(acc));
				sb::append(s, provider->at(path + std::string("objective_timespan"))(acc));
				//sb::layout_horizontal(s);
				//sb::unborder(s);
				return s;
			};
		}

		std::unique_ptr< value_objective > value_objective_defn::generate(
			prm::param_accessor acc,
			std::function< double(state_value_id) > get_state_value_fn
			)
		{
			acc.move_to(acc.find_path("value_objective"));

			std::unique_ptr< value_objective > obj;

			auto dir_type_str = prm::extract_as< prm::enum_param_val >(acc["direction_type"])[0];
			auto dir_type = value_objective::DirectionTypeMp.at(dir_type_str);

			auto state_val_str = prm::extract_as< prm::enum_param_val >(acc["state_val"])[0];
			auto state_val_id = state_value_id::from_string(state_val_str);

			auto value_type_str = prm::extract_as< prm::enum_param_val >(acc["value_type"])[0];
			auto value_type = value_objective::ValueTypeMp.at(value_type_str);

			acc.move_to(acc.find_path("objective_timespan"));

			switch(value_type)
			{
				case value_objective::ValueType::ValueAt:
				{
					auto tp_type_str = prm::extract_as< prm::enum_param_val >(acc["timepoint_type"])[0];
					auto tp_type = value_objective::TimePointTypeMp.at(tp_type_str);
					obj = std::make_unique< single_timepoint_objective >(dir_type, state_val_id, value_type, tp_type);
				}
				break;

				default:
				{
					auto from_tp_type_str = prm::extract_as< prm::enum_param_val >(acc["from_timepoint_type"])[0];
					auto from_tp_type = value_objective::TimePointTypeMp.at(from_tp_type_str);
					auto until_tp_type_str = prm::extract_as< prm::enum_param_val >(acc["until_timepoint_type"])[0];
					auto until_tp_type = value_objective::TimePointTypeMp.at(until_tp_type_str);
					obj = std::make_unique< timespan_objective >(dir_type, state_val_id, value_type, from_tp_type, until_tp_type);
				}
				break;
			}

			obj->set_stateval_fn(get_state_value_fn);

			acc.revert();
			acc.revert();
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


