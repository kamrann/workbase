// rtp_manual_objective.h

#ifndef __NE_RTP_MANUAL_OBJECTIVE_H
#define __NE_RTP_MANUAL_OBJECTIVE_H

#include "systems/rtp_system_state_values.h"

#include "wt_param_widgets/pw_fwd.h"

#include <boost/optional.hpp>

#include <string>
#include <map>
#include <vector>


namespace YAML {
	class Node;
}


namespace rtp {

	class manual_objective
	{
	public:
		enum class DirectionType {
			Minimise,	// Want small values
			Maximise,	// Want large values

			//		Count,
			//		Default = Minimise,
		};

		static const DirectionType DefaultDirectionType = DirectionType::Minimise;

		enum class ValueType {
			ValueAt,	// Value at a single time point
			Average,	// Average over time period
			Sum,		// Sum over time period
			Minimum,	// Minimum during time period
			Maximum,	// Maximum during time period

			//		Count,
			//		Default = ValueAt,
		};

		static const ValueType DefaultValueType = ValueType::ValueAt;

		enum class TimePointType {
			SimulationStart,
			SimulationEnd,
			WhenFirst,			// At first point a condition is satisfied
			WhenLast,			// AT last point a condition is satisfied

			//		Count,
			//		Default = SimulationEnd,
		};

		//	static const TimePointType DefaultTimePointType = TimePointType::SimulationEnd;

		/*	static std::string const DirectionTypeNames[DirectionType::Count];
			static std::string const ValueTypeNames[ValueType::Count];
			static std::string const TimePointTypeNames[TimePointType::Count];
			*/
		static std::map< DirectionType, std::string > const DirectionTypeNames;
		static std::map< ValueType, std::string > const ValueTypeNames;
		static std::map< TimePointType, std::string > const TimePointTypeNames;

	public:
		static std::string update_schema_providor(prm::schema::schema_provider_map_handle provider, prm::qualified_path const& prefix);
		static std::unique_ptr< manual_objective > create_instance(prm::param_accessor param, std::function< double(state_value_id) > get_state_value_fn);

	public:
		manual_objective(
			DirectionType direction_type,
			state_value_id val_id,
			ValueType value_type
			);

	public:
		void set_stateval_fn(std::function< double(state_value_id) > fn)
		{
			m_get_state_fn = fn;
		}

		virtual void reset();
		virtual void update();
		virtual void finalise();

		bool has_value() const
		{
			return m_value;
		}
		boost::optional< double > get_objective_value() const
		{
			return m_value;
		}

	protected:
		class timepoint_data
		{
		public:
			timepoint_data(TimePointType type): m_type(type)
			{}

		public:
			inline TimePointType type() const
			{
				return m_type;
			}

		private:
			TimePointType m_type;
			// TODO: Condition data for WhenFirst, WhenLast
		};

		class timepoint_state
		{
		public:
			timepoint_state(): m_fixed(false), m_active(false)
			{}

		public:
			inline void set_fixed()
			{
				set_active();
				m_fixed = true;
			}

			inline void set_active()
			{
				m_active = true;
			}

			inline bool is_fixed() const
			{
				return m_fixed;
			}

			inline bool is_active() const
			{
				return m_active;
			}

		private:
			bool m_fixed;
			bool m_active;
		};

	protected:
		bool test_timepoint(timepoint_data const& tp_data, timepoint_state& tp_state);
		inline double get_state_value()
		{
			return m_get_state_fn(m_state_val_id);
		}

	protected:
		DirectionType m_dir_type;
		state_value_id m_state_val_id;
		std::function< double(state_value_id) > m_get_state_fn;
		ValueType m_val_type;

		size_t m_update_counter;
		boost::optional< double > m_value;
	};


	class single_timepoint_objective: public manual_objective
	{
	public:
		single_timepoint_objective(
			DirectionType direction_type,
			state_value_id state_val_id,
			ValueType value_type,
			TimePointType timepoint_type);

	public:
		void reset();
		void update();

	private:
		timepoint_data m_tp_data;
		timepoint_state m_tp_state;
	};

	class timespan_objective: public manual_objective
	{
	public:
		timespan_objective(
			DirectionType direction_type,
			state_value_id state_val_id,
			ValueType value_type,
			TimePointType from_timepoint_type,
			TimePointType to_timepoint_type);

	public:
		void reset();
		void update();
		void finalise();

	protected:
		void update_value();

	private:
		timepoint_data m_from_tp_data;
		timepoint_state m_from_tp_state;
		timepoint_data m_to_tp_data;
		timepoint_state m_to_tp_state;
		size_t m_span_updates;
	};

}


#endif

