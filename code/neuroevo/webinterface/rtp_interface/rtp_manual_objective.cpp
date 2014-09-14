// rtp_manual_objective.cpp

#include "rtp_manual_objective.h"
#include "systems/rtp_system.h"

#include "wt_param_widgets/pw_yaml.h"
#include "wt_param_widgets/schema_builder.h"
#include "wt_param_widgets/param_accessor.h"


namespace rtp {

	std::map< manual_objective::DirectionType, std::string > const manual_objective::DirectionTypeNames = {
			{ DirectionType::Minimise, "Minimise" },
			{ DirectionType::Maximise, "Maximise" },
	};

	std::map< manual_objective::ValueType, std::string > const manual_objective::ValueTypeNames = {
			{ ValueType::ValueAt, "Value" },
			{ ValueType::Average, "Average" },
			{ ValueType::Sum, "Sum" },
			{ ValueType::Minimum, "Minimum" },
			{ ValueType::Maximum, "Maximum" },
	};

	std::map< manual_objective::TimePointType, std::string > const manual_objective::TimePointTypeNames = {
			{ TimePointType::SimulationStart, "Start" },
			{ TimePointType::SimulationEnd, "End" },
			{ TimePointType::WhenFirst, "When First" },
			{ TimePointType::WhenLast, "When Last" },
	};

}

namespace YAML {

	template <>
	struct convert< rtp::manual_objective::DirectionType >
	{
		static Node encode(rtp::manual_objective::DirectionType const& rhs)
		{
			return Node(rtp::manual_objective::DirectionTypeNames.at(rhs));
		}

		static bool decode(Node const& node, rtp::manual_objective::DirectionType& rhs)
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

		static std::map< std::string, rtp::manual_objective::DirectionType > const mapping_;
	};

	std::map< std::string, rtp::manual_objective::DirectionType > const convert< rtp::manual_objective::DirectionType >::mapping_ = {
			{ "Minimise", rtp::manual_objective::DirectionType::Minimise },
			{ "Maximise", rtp::manual_objective::DirectionType::Maximise },
	};

	template <>
	struct convert< rtp::manual_objective::ValueType >
	{
		static Node encode(rtp::manual_objective::ValueType const& rhs)
		{
			return Node(rtp::manual_objective::ValueTypeNames.at(rhs));
		}

		static bool decode(Node const& node, rtp::manual_objective::ValueType& rhs)
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

		static std::map< std::string, rtp::manual_objective::ValueType > const mapping_;
	};

	std::map< std::string, rtp::manual_objective::ValueType > const convert< rtp::manual_objective::ValueType >::mapping_ = {
			{ "Value", rtp::manual_objective::ValueType::ValueAt },
			{ "Average", rtp::manual_objective::ValueType::Average },
			{ "Sum", rtp::manual_objective::ValueType::Sum },
			{ "Minimum", rtp::manual_objective::ValueType::Minimum },
			{ "Maximum", rtp::manual_objective::ValueType::Maximum },
	};

	template <>
	struct convert< rtp::manual_objective::TimePointType >
	{
		static Node encode(rtp::manual_objective::TimePointType const& rhs)
		{
			return Node(rtp::manual_objective::TimePointTypeNames.at(rhs));
		}

		static bool decode(Node const& node, rtp::manual_objective::TimePointType& rhs)
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

		static std::map< std::string, rtp::manual_objective::TimePointType > const mapping_;
	};

	std::map< std::string, rtp::manual_objective::TimePointType > const convert< rtp::manual_objective::TimePointType >::mapping_ = {
			{ "Start", rtp::manual_objective::TimePointType::SimulationStart },
			{ "End", rtp::manual_objective::TimePointType::SimulationEnd },
			{ "When First", rtp::manual_objective::TimePointType::WhenFirst },
			{ "When Last", rtp::manual_objective::TimePointType::WhenLast },
	};
}


namespace rtp {

	namespace sb = prm::schema;

	std::string manual_objective::update_schema_providor(prm::schema::schema_provider_map_handle provider, prm::qualified_path const& prefix)
	{
		auto relative = std::string{ "manual_objective" };
		auto path = prefix + relative;

		(*provider)[path + std::string("direction_type")] = [](prm::param_accessor)
		{
			auto dir_type_name_list = std::vector< std::string >(DirectionTypeNames.size());
			std::transform(begin(DirectionTypeNames), end(DirectionTypeNames), begin(dir_type_name_list), [](std::pair< DirectionType, std::string > const& val)
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

		(*provider)[path + std::string("state_val")] = [](prm::param_accessor param_vals)
		{
			// TODO: Ultimately needs to be a list of all available system state values
			// (may need to call system::create_instance() with already specified params in order to
			// retrieve these.

			auto compatible_node = param_vals["compatible_agent_types"];

			auto num_available_inputs = 0;
			std::vector< std::pair< std::string, std::string > > state_vals;

			bool is_attached_instance = compatible_node.size() > 0;
			////// TODO: Temp, while this is limited to single selection
			is_attached_instance = is_attached_instance && !compatible_node.as< prm::is_unspecified >();
			//////
			if(is_attached_instance)
			{
				// TODO: For now assuming single selection
				auto system_val_names = i_system::get_system_state_values(param_vals);
				auto sys_type = param_vals["sys_type"][0].as< SystemType >();
				auto agent_val_names = i_system::get_agent_sensor_names(sys_type, compatible_node[0], param_vals);

				std::transform(
					std::begin(system_val_names),
					std::end(system_val_names),
					std::back_inserter(state_vals),
					[](std::string const& name)
				{
					std::stringstream data;
					data << "::system::" << name;
					return std::make_pair(data.str(), name);
				});
				
				size_t idx = 0;
				std::transform(
					std::begin(agent_val_names),
					std::end(agent_val_names),
					std::back_inserter(state_vals),
					[idx](std::string const& name) mutable
				{
					std::stringstream data;
					data << "::agent::" << idx++;
					return std::make_pair(data.str(), name);
				});
			}

			auto s = sb::enum_selection(
				"state_val",
				state_vals,
				0, 1
				);
			sb::unlabel(s);
			sb::update_on(s, "evolved_controller_compatibility_changed");
			return s;
		};

		(*provider)[path + std::string("value_type")] = [](prm::param_accessor)
		{
			auto value_type_name_list = std::vector< std::string >(ValueTypeNames.size());
			std::transform(begin(ValueTypeNames), end(ValueTypeNames), begin(value_type_name_list), [](std::pair< ValueType, std::string > const& val)
			{
				return val.second;
			});
			auto s = sb::enum_selection(
				"value_type",
				value_type_name_list
				);
			sb::unlabel(s);
			sb::trigger(s, "obj_value_type_changed");
			return s;
		};


		path += std::string("objective_timespan");

		(*provider)[path + std::string("timepoint_type")] = [](prm::param_accessor)
		{
			auto s = sb::enum_selection(
				"timepoint_type",
				{
				TimePointTypeNames.at(TimePointType::SimulationEnd),
				TimePointTypeNames.at(TimePointType::WhenFirst),
				TimePointTypeNames.at(TimePointType::WhenLast),
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
				TimePointTypeNames.at(TimePointType::SimulationStart),
				TimePointTypeNames.at(TimePointType::WhenFirst),
				TimePointTypeNames.at(TimePointType::WhenLast),
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
				TimePointTypeNames.at(TimePointType::SimulationEnd),
				TimePointTypeNames.at(TimePointType::WhenFirst),
				TimePointTypeNames.at(TimePointType::WhenLast),
				}
			);
			sb::label(s, "and");
			return s;
		};

		(*provider)[path] = [=](prm::param_accessor param_vals)
		{
			auto s = sb::list("objective_timespan");

			auto value_type_node = param_vals["value_type"];
			auto value_type = !value_type_node ? DefaultValueType : value_type_node[0].as< ValueType >();
			switch(value_type)
			{
				case ValueType::ValueAt:
				sb::append(s, provider->at(path + std::string("timepoint_type"))(param_vals));
				break;

				default:
				sb::append(s, provider->at(path + std::string("from_timepoint_type"))(param_vals));
				sb::append(s, provider->at(path + std::string("until_timepoint_type"))(param_vals));
				break;
			}

			sb::update_on(s, "obj_value_type_changed");
			sb::unborder(s);
			sb::layout_horizontal(s);
			return s;
		};

		path.pop();


		(*provider)[path] = [=](prm::param_accessor param_vals)
		{
			auto s = sb::list(relative);
			sb::append(s, provider->at(path + std::string("direction_type"))(param_vals));
			sb::append(s, provider->at(path + std::string("state_val"))(param_vals));
			sb::append(s, provider->at(path + std::string("value_type"))(param_vals));
			sb::append(s, provider->at(path + std::string("objective_timespan"))(param_vals));
			sb::layout_horizontal(s);
			sb::unborder(s);
			return s;
		};

		return relative;
}

	std::unique_ptr< manual_objective > manual_objective::create_instance(
		prm::param_accessor param,
		std::function< double(state_value_id) > get_state_value_fn)
	{
		param.push_relative_path(prm::qualified_path("manual_objective"));

		std::unique_ptr< manual_objective > obj;

		auto dir_type = param["direction_type"][0].as< DirectionType >();

		auto state_val_node = param["state_val"];
		if(state_val_node.as< prm::is_unspecified >())
		{
			throw prm::required_unspecified("Objective State Value");
		}
		auto state_val_ep = state_val_node.as< prm::enum_param_val >();
		auto state_val_id_string = boost::any_cast<std::vector< std::string >>(state_val_ep.contents)[0];
		auto state_val_id = state_val_id_string;//std::stoi();

		auto value_type = param["value_type"][0].as< ValueType >();

		param.push_relative_path(prm::qualified_path("objective_timespan"));

		switch(value_type)
		{
			case ValueType::ValueAt:
			{
				auto tp_type = param["timepoint_type"][0].as< TimePointType >();
				obj = std::make_unique< single_timepoint_objective >(dir_type, state_val_id, value_type, tp_type);
			}
			break;

			default:
			{
				auto from_tp_type = param["from_timepoint_type"][0].as< TimePointType >();
				auto until_tp_type = param["until_timepoint_type"][0].as< TimePointType >();
				obj = std::make_unique< timespan_objective >(dir_type, state_val_id, value_type, from_tp_type, until_tp_type);
			}
			break;
		}

		param.pop_relative_path();

		obj->m_get_state_fn = get_state_value_fn;

		param.pop_relative_path();
		return obj;
	}


	manual_objective::manual_objective(
		DirectionType direction_type,
		state_value_id state_val_id,
		ValueType value_type
		):
		m_dir_type(direction_type),
		m_state_val_id(state_val_id),
		m_val_type(value_type)
	{
		reset();
	}

	void manual_objective::reset()
	{
		m_update_counter = 0;
		m_value = boost::none;
	}

	void manual_objective::update()
	{
		++m_update_counter;
	}

	void manual_objective::finalise()
	{
		if(m_value && m_dir_type == DirectionType::Minimise)
		{
			*m_value = -*m_value;
		}
	}

	bool manual_objective::test_timepoint(timepoint_data const& tp_data, timepoint_state& tp_state)
	{
		switch(tp_data.type())
		{
			case TimePointType::SimulationStart:
			tp_state.set_fixed();
			return m_update_counter == 0;

			case TimePointType::SimulationEnd:
			return true;

			case TimePointType::WhenFirst:
			if(false)	// tp_data.test_condition())
			{
				tp_state.set_fixed();
				return true;
			}
			else
			{
				return false;
			}

			case TimePointType::WhenLast:
			return false;	// tp_data.test_condition();

			default:
			throw std::exception("Invalid TimePointType");
		}
	}


	single_timepoint_objective::single_timepoint_objective(
		DirectionType direction_type,
		state_value_id state_val_id,
		ValueType value_type,
		TimePointType timepoint_type
		):
		manual_objective(direction_type, state_val_id, value_type),
		m_tp_data(timepoint_type)
	{}

	void single_timepoint_objective::reset()
	{
		manual_objective::reset();

		m_tp_state = timepoint_state();
	}

	void single_timepoint_objective::update()
	{
		if(!m_tp_state.is_fixed())
		{
			if(test_timepoint(m_tp_data, m_tp_state))
			{
				m_value = get_state_value();
			}
		}

		manual_objective::update();
	}


	timespan_objective::timespan_objective(
		DirectionType direction_type,
		state_value_id state_val_id,
		ValueType value_type,
		TimePointType from_timepoint_type,
		TimePointType to_timepoint_type
		):
		manual_objective(direction_type, state_val_id, value_type),
		m_from_tp_data(from_timepoint_type),
		m_to_tp_data(to_timepoint_type)
	{}

	void timespan_objective::reset()
	{
		manual_objective::reset();

		m_from_tp_state = timepoint_state();
		m_to_tp_state = timepoint_state();
		m_span_updates = 0;
	}

	void timespan_objective::update_value()
	{
		auto current = get_state_value();
		switch(m_val_type)
		{
			case ValueType::Average:
			++m_span_updates;
			case ValueType::Sum:
			*m_value += current;
			return;

			case ValueType::Minimum:
			m_value = m_value ? std::min(*m_value, current) : current;
			return;

			case ValueType::Maximum:
			m_value = m_value ? std::max(*m_value, current) : current;
			return;
		}
	}

	void timespan_objective::update()
	{
		if(m_to_tp_state.is_fixed())
		{
			return;	// ??
		}

		// assuming is_fixed() implies is_active()
		if(!m_from_tp_state.is_fixed() && test_timepoint(m_from_tp_data, m_from_tp_state))
		{
			// from point reset
			/*
			if(is_fixed(to_point))
			{
			error?
			}
			*/

			// reset value
			m_value = get_state_value();
			m_span_updates = 1;
			m_from_tp_state.set_active();
		}
		else if(m_from_tp_state.is_active())
		{
			if(test_timepoint(m_to_tp_data, m_to_tp_state))
			{
				update_value();
			}
		}

		manual_objective::update();
	}

	void timespan_objective::finalise()
	{
		if(has_value())
		{
			switch(m_val_type)
			{
				case ValueType::Average:
				{
					*m_value /= m_span_updates;
				}
			}
		}
	}

}

