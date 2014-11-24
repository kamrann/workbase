// value_objective.h

#ifndef __WB_EV_SYS_VALUE_OBJECTIVE_H
#define __WB_EV_SYS_VALUE_OBJECTIVE_H

#include "system_sim/system_state_values.h"

#include "params/param_fwd.h"

#include "util/bimap.h"

#include <boost/optional.hpp>

#include <string>
#include <map>
#include <vector>
#include <functional>


namespace sys {
	namespace ev {

		class value_objective
		{
		public:
			enum class DirectionType {
				Minimise,	// Want small values
				Maximise,	// Want large values
			};

			static const DirectionType DefaultDirectionType = DirectionType::Minimise;

			enum class ValueType {
				ValueAt,	// Value at a single time point
				Average,	// Average over time period
				Sum,		// Sum over time period
				Minimum,	// Minimum during time period
				Maximum,	// Maximum during time period
			};

			static const ValueType DefaultValueType = ValueType::ValueAt;

			enum class TimePointType {
				SimulationStart,
				SimulationEnd,
				WhenFirst,			// At first point a condition is satisfied
				WhenLast,			// AT last point a condition is satisfied
			};

			//	static const TimePointType DefaultTimePointType = TimePointType::SimulationEnd;

			typedef bimap< DirectionType, std::string > direction_type_map_t;
			static direction_type_map_t const DirectionTypeMp;
			typedef bimap< ValueType, std::string > value_type_map_t;
			static value_type_map_t const ValueTypeMp;
			typedef bimap< TimePointType, std::string > timepoint_type_map_t;
			static timepoint_type_map_t const TimePointTypeMp;

		public:
			value_objective(
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


		class single_timepoint_objective: public value_objective
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

		class timespan_objective: public value_objective
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


		class value_objective_defn
		{
		public:
			typedef std::function< state_value_id_list(prm::param_accessor) > state_val_access_fn_t;

		public:
			value_objective_defn(state_val_access_fn_t sv_acc_fn);

		public:
			void update_schema_provider(prm::schema::schema_provider_map_handle provider, prm::qualified_path const& prefix);
			std::unique_ptr< value_objective > generate(prm::param_accessor acc, std::function< double(state_value_id) > get_state_value_fn);

		protected:
			state_val_access_fn_t sv_acc_fn_;
		};

	}
}



#endif

