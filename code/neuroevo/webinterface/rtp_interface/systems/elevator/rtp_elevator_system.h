// rtp_elevator_system.h

#ifndef __NE_RTP_ELEVATOR_SYSTEM_H
#define __NE_RTP_ELEVATOR_SYSTEM_H

#include "../rtp_system.h"
#include "../rtp_agent.h"

#include "wt_param_widgets/pw_fwd.h"

#include <string>
#include <array>
#include <vector>
#include <memory>
#include <random>


namespace rtp {

//	class i_agent;
	class i_controller;
	class i_observer;
	class i_properties;
	class i_property_values;
	class i_system_drawer;

	struct elevator_agent_specification
	{};

	class elevator_system:
		public i_system,
		public i_agent
	{
	public:
		static std::string update_schema_providor(prm::schema::schema_provider_map_handle provider, prm::qualified_path const& prefix, bool evolvable);

		static std::unique_ptr< i_system_factory > generate_factory(prm::param_accessor param_vals);
		static agents_data generate_agents_data(prm::param_accessor param_vals, bool evolvable);

		static std::vector< std::string > get_state_values(prm::param_accessor param_vals);
		static std::vector< std::string > get_agent_type_names();
		static std::vector< std::string > get_agent_sensor_names(prm::param agent_type, prm::param_accessor param_vals);
		static size_t get_agent_num_effectors(prm::param agent_type);

	protected:
		typedef unsigned int floor_count_t, floor_t;
		typedef double sys_time_t, sys_duration_t;
		typedef double arrival_rate_t;

		enum class ButtonState {
			Unpressed,
			Pressed,
		};

		enum Direction {
			Up,
			Down,

			None,
		};

		struct floor_button_state
		{
			ButtonState state;
			sys_time_t pressed_time;

			floor_button_state():
				state(ButtonState::Unpressed),
				pressed_time(0)
			{}
		};

		struct floor_state
		{
			// Up/Down button states
			std::array< floor_button_state, 2 > buttons;

			struct waiting_person
			{
				floor_t destination;
				sys_time_t arrival_time;
			};

			typedef std::vector< waiting_person > queue_t;

			std::array< queue_t, 2 > queues;

			floor_state()
			{
				queues[Direction::Up].reserve(10);
				queues[Direction::Down].reserve(10);
			}

			inline bool is_pressed(Direction dir) const
			{
				return buttons[dir].state != ButtonState::Unpressed;
			}

			inline size_t waiting_up() const
			{
				return queues[Direction::Up].size();
			}

			inline size_t waiting_down() const
			{
				return queues[Direction::Down].size();
			}

			inline size_t waiting() const
			{
				return waiting_up() + waiting_down();
			}
		};

		struct passenger
		{
			sys_time_t queue_arrival_time;
			sys_time_t boarding_time;
		};

		typedef std::vector< passenger > passenger_list;
			
		struct passenger_data
		{
			size_t count;
			passenger_list passengers;

			passenger_data():
				count(0)
			{}
		};

		struct system_state
		{
			typedef std::vector< floor_state > floor_states;
			//typedef std::vector< size_t > passenger_destination_counts;
			typedef std::vector< passenger_data > by_dest_passenger_data;

			floor_t location;
			Direction direction;
			floor_states floors;
			//passenger_destination_counts passenger_destinations;	// Dest counts for each floor of onboard passengers only
			by_dest_passenger_data passengers_by_dest;
			size_t occupancy;

			system_state():
				location(0),
				direction(Direction::None),
				occupancy(0)
			{}

			void initialize(floor_count_t num_floors)
			{
				floors.resize(num_floors);
				passengers_by_dest.resize(num_floors);
			}

			inline bool at_top() const
			{
				return location == floors.size() - 1;
			}

			inline bool at_bottom() const
			{
				return location == 0;
			}

			inline bool is_internal_floor_request() const
			{
				return std::any_of(
					std::begin(passengers_by_dest),
					std::end(passengers_by_dest),
					[](passenger_data const& pd)
				{
					return pd.count > 0;
				});
			}

			inline bool is_external_floor_request() const
			{
				return std::any_of(
					std::begin(floors),
					std::end(floors),
					[](floor_state const& flr)
				{
					return flr.is_pressed(Direction::Up) || flr.is_pressed(Direction::Down);
				});
			}

			inline bool is_any_floor_request() const
			{
				return is_internal_floor_request() || is_external_floor_request();
			}

			inline size_t get_num_waiting() const
			{
				size_t count = 0;
				for(auto const& f : floors)
				{
					count += f.waiting_up();
					count += f.waiting_down();
				}
				return count;
			}
		};

		struct decision
		{
			Direction direction;
			bool stop;		// If true, allows people on/off at current floor, and direction
							// signifies the onward travel direction after the stop.

			decision():
				direction(Direction::None),
				stop(false)
			{}

			decision(Direction _dir, bool _stop):
				direction(_dir),
				stop(_stop)
			{}
		};

		struct transition_data
		{
			std::vector< std::array< size_t, 2 > > floor_arrivals;
			size_t num_got_off;
			size_t num_got_on;

			transition_data():
				num_got_off(0),
				num_got_on(0)
			{}

			void initialize(floor_count_t num_floors)
			{
				floor_arrivals.resize(num_floors, { { 0ul, 0ul } });
			}
		};

		struct history_data
		{
			size_t num_people_arrived;
			size_t num_people_boarded;
			size_t num_people_delivered;
			size_t distance_moved;	// in floors
			// From arrival to boarding time, only for completed boardings
			sys_duration_t total_queuing_time;
			sys_duration_t max_queuing_time;
			// And inclusive of people not yet boarded
			sys_duration_t total_inclusive_queuing_time;
			// From arrival right through to destination arrival time, only for completed deliveries
			sys_duration_t total_delivery_time;
			sys_duration_t max_delivery_time;
			// And inclusive of people not delivered
			sys_duration_t total_inclusive_delivery_time;
			// Time the elevator is stationary
			sys_duration_t idle_time;

			history_data():
				num_people_arrived(0),
				num_people_boarded(0),
				num_people_delivered(0),
				distance_moved(0),
				total_queuing_time(0),
				max_queuing_time(0),
				total_inclusive_queuing_time(0),
				total_delivery_time(0),
				max_delivery_time(0),
				total_inclusive_delivery_time(0),
				idle_time(0)
			{}
		};

		static inline bool is_moving(Direction dir)
		{
			return dir != Direction::None;
		}

		// Assumes from and to are never the same
		static inline Direction get_direction(const floor_t from, const floor_t to)
		{
			return from > to ? Direction::Down : Direction::Up;
		}

		// Assumes valid, ie. no impossible combinations such as 0, Down
		static inline floor_t get_next_floor(const floor_t from, const Direction dir)
		{
			switch(dir)
			{
				case Direction::Up:
				return from + 1;
				case Direction::Down:
				return from - 1;
				default:
				return from;
			}
		}

		// Assumes valid, ie. no impossible combinations such as 0, Down
		static inline floor_t get_prev_floor(const floor_t from, const Direction dir)
		{
			switch(dir)
			{
				case Direction::Up:
				return from - 1;
				case Direction::Down:
				return from + 1;
				default:
				return from;
			}
		}

		static std::string floor_name(floor_t floor);
		static std::string direction_name(Direction dir);

	public:
		elevator_system(prm::param_accessor params);
		~elevator_system();

	public:
		virtual update_info get_update_info() const override;

		virtual void initialize() override;
		virtual void clear_agents() override;
		virtual agent_id register_agent(agent_specification const& spec) override;
		virtual void register_agent_controller(agent_id agent, std::unique_ptr< i_controller > controller) override;
		
		virtual i_agent const& get_agent(agent_id id) const;
		virtual i_controller const& get_agent_controller(agent_id id) const;

		virtual bool update(i_observer* obs);
		virtual void register_interactive_input(interactive_input const& input);
		virtual i_observer::observations_t record_observations(i_observer* obs) const;

		virtual std::shared_ptr< i_properties const > get_state_properties() const;
		virtual std::shared_ptr< i_property_values const > get_state_property_values() const;
		virtual double get_state_value(state_value_id id) const override;
		virtual std::unique_ptr< i_system_drawer > get_drawer() const;

		//
		virtual std::string get_name() const override;
		virtual agent_sensor_list get_mapped_inputs(std::vector< std::string > const& named_inputs) const override;
		virtual double get_sensor_value(agent_sensor_id const& sensor) const override;

	protected:
		void process_arrivals();
		void process_stop(Direction next_direction);
		decision make_decision();
		void validate_decision(decision& dec) const;
		void update_elevator();

	public:
		floor_count_t m_num_floors;
		arrival_rate_t m_arrival_rate;
		sys_time_t m_time;
		sys_duration_t m_update_period;
		system_state m_state;
		transition_data m_transitions;	// TODO: Idea is this stuff is for purposes of feedback/display only,
			// and so can be disabled for learning runs to save time.
		history_data m_history;

		std::exponential_distribution<> m_arrival_dist;

		std::unique_ptr< i_controller > m_controller;

		static std::vector< std::string > generate_state_value_names(floor_count_t num_floors);
		static std::map< std::string, agent_sensor_id > generate_state_value_idmap(floor_count_t num_floors);

		std::vector< std::string > m_state_value_names;
		std::map< std::string, agent_sensor_id > m_state_value_ids;

		void initialize_state_value_accessors();

		typedef std::function< double() > state_value_accessor_fn;
		std::vector< state_value_accessor_fn > m_state_value_accessors;

	private:
/*
//#ifdef PERF_LOGGING
	public:
		std::chrono::high_resolution_clock::duration
			m_agent_update_time,
			m_b2d_update_time,
			m_observer_update_time;
*/
		virtual void concatenate_performance_data(perf_data_t& pd) const override;
//#endif

		friend class elevator_system_drawer;
	};

}


#endif

