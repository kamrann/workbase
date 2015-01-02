// value_objective.cpp

#include "value_objective.h"
//#include "systems/rtp_system.h"


namespace sys {
	namespace ev {

		bimap< value_objective::DirectionType, std::string > const value_objective::DirectionTypeMp = {
				{ DirectionType::Minimise, "Minimise" },
				{ DirectionType::Maximise, "Maximise" },
		};

		bimap< value_objective::ValueType, std::string > const value_objective::ValueTypeMp = {
				{ ValueType::ValueAt, "Value" },
				{ ValueType::Average, "Average" },
				{ ValueType::Sum, "Sum" },
				{ ValueType::Minimum, "Minimum" },
				{ ValueType::Maximum, "Maximum" },
		};

		bimap< value_objective::TimePointType, std::string > const value_objective::TimePointTypeMp = {
				{ TimePointType::SimulationStart, "Start" },
				{ TimePointType::SimulationEnd, "End" },
				{ TimePointType::WhenFirst, "When First" },
				{ TimePointType::WhenLast, "When Last" },
		};


		value_objective::value_objective(
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

		void value_objective::reset()
		{
			m_update_counter = 0;
			m_value = boost::none;
		}

		void value_objective::update()
		{
			++m_update_counter;
		}

		void value_objective::finalise()
		{
			if(m_value && m_dir_type == DirectionType::Minimise)
			{
				*m_value = -*m_value;
			}
		}

		bool value_objective::test_timepoint(timepoint_data const& tp_data, timepoint_state& tp_state)
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
			value_objective(direction_type, state_val_id, value_type),
			m_tp_data(timepoint_type)
		{}

		void single_timepoint_objective::reset()
		{
			value_objective::reset();

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

			value_objective::update();
		}


		timespan_objective::timespan_objective(
			DirectionType direction_type,
			state_value_id state_val_id,
			ValueType value_type,
			TimePointType from_timepoint_type,
			TimePointType to_timepoint_type
			):
			value_objective(direction_type, state_val_id, value_type),
			m_from_tp_data(from_timepoint_type),
			m_to_tp_data(to_timepoint_type)
		{}

		void timespan_objective::reset()
		{
			value_objective::reset();

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

			value_objective::update();
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

			value_objective::finalise();
		}

	}
}


