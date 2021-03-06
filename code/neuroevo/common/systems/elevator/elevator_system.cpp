// elevator_system.cpp

#include "elevator_system.h"
#include "elevator_system_defn.h"
#include "elevator_systemstatevalues.h"

#include "system_sim/basic_agent.h"
#include "system_sim/controller.h"

#include "params/param_accessor.h"


namespace sys {
	namespace elev {

		std::string elevator_system::direction_name(Direction dir)
		{
			switch(dir)
			{
				case Direction::Up:
				return "Up";
				case Direction::Down:
				return "Down";
				default:
				return "Nowhere";
			}
		}


		elevator_system::elevator_system(prm::param_accessor params)
		{
			// TODO: extract_as currently silently fails (applies default value) if use size_t or other non-int
			m_num_floors = prm::extract_as< int >(params["floor_count"]);
			m_arrival_rate = prm::extract_as< arrival_rate_t >(params["arrival_rate"]);

			//m_state_value_names = elevator_system_defn::generate_state_value_names(m_num_floors);
			//m_state_value_ids = elevator_system_defn::generate_state_value_idmap(m_num_floors);

			//initialize_state_value_accessors();

			m_time = 0.0;
			m_update_period = 1.0;
		}

		elevator_system::~elevator_system()
		{

		}

		bool elevator_system::initialize()
		{
			if(!basic_system::initialize())
			{
				return false;
			}

			m_state = system_state();
			m_state.initialize(m_num_floors);

			m_transitions = transition_data();
			m_transitions.initialize(m_num_floors);

			m_time = 0.0;

			if(m_arrival_rate > 0.0)
			{
				m_arrival_dist = std::exponential_distribution<>(m_arrival_rate);
			}

			return true;
		}

		void elevator_system::clear_agents()
		{
			// TODO: needed?
		}


		i_system::agent_id elevator_system::register_agent(agent_ptr agent)
		{
			m_agent = std::move(agent);
			return 0;
		}

		void elevator_system::register_agent_controller(agent_id agent, controller_ptr controller)
		{
			m_controller = std::move(controller);
		}

		i_agent const& elevator_system::get_agent(agent_id id) const
		{
			return *m_agent;
		}

		i_controller const& elevator_system::get_agent_controller(agent_id id) const
		{
			return *m_controller;
		}

		void elevator_system::register_interactive_input(interactive_input const& input)
		{
			// TODO: Need to either pass in boolean is_interactive argument to register_agent_controller(),
			// or provide a method i_controller::is_interactive()

			/*		for(auto& entry : m_agents)
					{
					// Temp hack, see above
					auto as_interactive = dynamic_cast<i_interactive_controller*>(entry.controller.get());
					if(as_interactive != nullptr)
					{
					as_interactive->register_input(input);
					}
					}
					*/
		}


		void elevator_system::process_arrivals()
		{
			if(m_arrival_rate == 0.0)
			{
				return;
			}

			// TODO: Might it be faster to if possible just sample the number of arrivals in a given period directly, 
			// then sample their arrival times uniformly over the period?

			//
			std::uniform_int_distribution< floor_t > floor_dist(0, m_num_floors - 1);
			std::uniform_int_distribution< floor_t > dest_floor_dist(0, m_num_floors - 2);
			//

			double t = m_arrival_dist(m_rgen);
			while(t < m_update_period)
			{
				// Have an arrival at time t from the start of the period
				// For now, randomly choose arrival floor...
				floor_t floor = floor_dist(m_rgen);
				// And randomly choose destination
				floor_t destination = dest_floor_dist(m_rgen);
				// If get same as arrival floor, replace with top floor (which has been omitted from dest floor distribution)
				if(destination == floor)
				{
					destination = m_num_floors - 1;
				}
				auto direction = get_direction(floor, destination);

				auto& f_state = m_state.floors[floor];
				if(!f_state.is_pressed(direction))
				{
					f_state.buttons[direction].state = ButtonState::Pressed;
					f_state.buttons[direction].pressed_time = m_time;// +t;	TODO: For now, not dealing with intra-update times
				}
				f_state.queues[direction].emplace_back(floor_state::waiting_person{ destination, m_time/* + t*/ });

				m_transitions.floor_arrivals[floor][direction] += 1;
				m_history.num_people_arrived += 1;

				t += m_arrival_dist(m_rgen);
			}
		}

		void elevator_system::process_stop(Direction next_direction)
		{
			auto const floor = m_state.location;

#ifdef _DEBUG
			std::cout << "Opening doors..." << std::endl;
			std::cout << m_state.passengers_by_dest[floor].count << " people getting off" << std::endl;
#endif

			// Let passengers off and clear internal elevator buttons for current floor
			m_transitions.num_got_off = m_state.passengers_by_dest[floor].count;
			m_state.occupancy -= m_transitions.num_got_off;
			for(auto const& pass : m_state.passengers_by_dest[floor].passengers)
			{
				auto const delivery_time = m_time - pass.queue_arrival_time;
				m_history.total_delivery_time += delivery_time;
				// Note that this cannot be brought out of the loop as is done below, because the first person in
				// the passenger list was the first to board, but may not have been the first to start queuing.
				if(delivery_time > m_history.max_delivery_time)
				{
					m_history.max_delivery_time = delivery_time;
				}
			}
			m_state.passengers_by_dest[floor].passengers.clear();
			m_state.passengers_by_dest[floor].count = 0;
			m_history.num_people_delivered += m_transitions.num_got_off;

			if(is_moving(next_direction))
			{
				// Clear floor Up/Down button as appropriate
				m_state.floors[floor].buttons[next_direction].state = ButtonState::Unpressed;
				m_state.floors[floor].buttons[next_direction].pressed_time = 0.0;

				auto& queue = m_state.floors[floor].queues[next_direction];
				if(!queue.empty())
				{
					// Allow people on
#ifdef _DEBUG
					std::stringstream ss;
					ss << "[ ";
#endif
					for(auto const& person : queue)
					{
						++m_state.passengers_by_dest[person.destination].count;
						m_state.passengers_by_dest[person.destination].passengers.emplace_back(
							passenger{ person.arrival_time, m_time });

						auto const queuing_time = m_time - person.arrival_time;
						m_history.total_queuing_time += queuing_time;

#ifdef _DEBUG
						ss << person.destination << " ";
#endif
					}

					auto const longest_queuing_time = m_time - queue.front().arrival_time;
					if(longest_queuing_time > m_history.max_queuing_time)
					{
						m_history.max_queuing_time = longest_queuing_time;
					}

					m_state.occupancy += queue.size();
					m_history.num_people_boarded += queue.size();
#ifdef _DEBUG
					ss << "]";
					std::cout << queue.size() << " people get on with destinations: " << ss.str() << std::endl;
#endif
					m_transitions.num_got_on = queue.size();
					queue.clear();
				}
			}
		}

		elevator_system::decision elevator_system::make_decision()
		{
			decision dec{ Direction::None, false };

			auto required_inputs = m_controller->get_input_ids();
			auto num_inputs = required_inputs.size();
			i_controller::input_list_t inputs;
			for(auto const& input_id : required_inputs)
			{
				auto bound = get_state_value_binding(input_id);
				auto value = get_state_value(bound);
				inputs.push_back(value);
					//m_agent->get_sensor_value(input_id));
			}

			auto outputs = m_controller->process(inputs);
			// TODO: hack for passive controller.
			// ideally effector bindings would deal with this
			if(outputs.empty())
			{
				return dec;
			}
			//

			if(outputs[0] > 0.5)
			{
				dec.direction = Direction::Up;
			}
			else if(outputs[0] < -0.5)
			{
				dec.direction = Direction::Down;
			}
			dec.stop = outputs[1] > 0.0;
			return dec;
		}

		void elevator_system::validate_decision(decision& dec) const
		{
			if(
				(dec.direction == Direction::Up && m_state.location == m_num_floors - 1) ||
				(dec.direction == Direction::Down && m_state.location == 0)
				)
			{
				dec.direction = Direction::None;
			}
		}

		void elevator_system::update_elevator()
		{
			auto dec = make_decision();
			validate_decision(dec);

#ifdef _DEBUG
			std::cout << "At " << elevator_system_defn::floor_name(m_state.location) << " carrying " << m_state.occupancy << " people" << std::endl;
			std::cout << "Going " << direction_name(dec.direction) << " next" << std::endl;
#endif

			if(dec.stop)
			{
				process_stop(dec.direction);
			}

			m_state.direction = dec.direction;
			switch(m_state.direction)
			{
				case Direction::Up:
				++m_state.location;
				m_history.distance_moved += 1;
				break;

				case Direction::Down:
				--m_state.location;
				m_history.distance_moved += 1;
				break;

				default:
				m_history.idle_time += m_update_period;
				break;
			}
		}

		bool elevator_system::update(i_observer* obs)
		{
			m_transitions = transition_data();
			m_transitions.initialize(m_num_floors);

			process_arrivals();

			//auto const initial_num_waiting = m_state.get_num_waiting();
			//auto const initial_num_in_system = initial_num_waiting + m_state.occupancy;

			update_elevator();

			// Consider anyone who boards/gets delivered on this step, to have not waited/been in the system this step.
			// Note that this is for consistency with current queuing/delivery time calculations, which can give people
			// times of 0. May want to change all this so that minimum queuing/delivery time is 1?
			auto const final_num_waiting = m_state.get_num_waiting();
			m_history.total_inclusive_queuing_time += final_num_waiting * m_update_period;

			auto const final_num_in_system = final_num_waiting + m_state.occupancy;
			m_history.total_inclusive_delivery_time += final_num_in_system * m_update_period;

#ifdef _DEBUG
			std::cout << std::endl << "Destination counts:" << std::endl;
			for(floor_t f = 0; f < m_num_floors; ++f)
			{
				std::cout << elevator_system_defn::floor_name(f) << ": " << m_state.passengers_by_dest[f].count << std::endl;
			}
			std::cout << std::endl;

			std::cout << "Floor queues:" << std::endl;
			for(floor_t f = 0; f < m_num_floors; ++f)
			{
				std::cout << elevator_system_defn::floor_name(f) << ": ";
				auto waiting_up = m_state.floors[f].waiting_up();
				auto waiting_down = m_state.floors[f].waiting_down();
				if(waiting_up > 0)
				{
					std::cout << waiting_up << " Up";
				}
				if(waiting_down > 0)
				{
					if(waiting_up > 0)
					{
						std::cout << ", ";
					}
					std::cout << waiting_down << " Down";
				}
				if(waiting_up == 0 && waiting_down == 0)
				{
					std::cout << "None";
				}
				std::cout << std::endl;
			}
			std::cout << std::endl;
#endif

			if(obs)
			{
				auto start_ = std::chrono::high_resolution_clock::now();

				obs->update(this);

				auto end_ = std::chrono::high_resolution_clock::now();
				//m_observer_update_time += end_ - start_;
			}

			m_time += m_update_period;

			return m_time < 20.0;
		}

		i_observer::observations_t elevator_system::record_observations(i_observer* obs) const
		{
			return obs->record_observations(i_observer::trial_data_t{});
		}

		size_t elevator_system::initialize_state_value_bindings(sv_bindings_t& bindings, sv_accessors_t& accessors)
		{
			typedef elevator_system_defn defn;

			auto bound_id = accessors.size();
			auto initial_count = bindings.size();
			
			bindings[state_value_id{ defn::s_core_state_values.left.at(defn::CoreStateValue::Time) }] = bound_id++;
			accessors.push_back([this]{ return m_time; });

			bindings[state_value_id{ defn::s_core_state_values.left.at(defn::CoreStateValue::Location) }] = bound_id++;
			accessors.push_back([this]{ return m_state.location; });

			bindings[state_value_id{ defn::s_core_state_values.left.at(defn::CoreStateValue::Direction) }] = bound_id++;
			accessors.push_back([this]
			{
				int dir_value = 0;
				switch(m_state.direction)
				{
					case Direction::Up:
					dir_value = 1;
					break;
					case Direction::Down:
					dir_value = -1;
					break;
				}
				return dir_value;
			});

			bindings[state_value_id{ defn::s_core_state_values.left.at(defn::CoreStateValue::Occupancy) }] = bound_id++;
			accessors.push_back([this]{ return m_state.occupancy; });

			bindings[state_value_id{ defn::s_core_state_values.left.at(defn::CoreStateValue::NumWaiting) }] = bound_id++;
			accessors.push_back([this]{ return m_state.get_num_waiting(); });

			bindings[state_value_id{ defn::s_core_state_values.left.at(defn::CoreStateValue::NumGotOff) }] = bound_id++;
			accessors.push_back([this]{ return m_transitions.num_got_off; });

			bindings[state_value_id{ defn::s_core_state_values.left.at(defn::CoreStateValue::NumGotOn) }] = bound_id++;
			accessors.push_back([this]{ return m_transitions.num_got_on; });

			bindings[state_value_id{ defn::s_core_state_values.left.at(defn::CoreStateValue::TotalArrivals) }] = bound_id++;
			accessors.push_back([this]{ return m_history.num_people_arrived; });

			bindings[state_value_id{ defn::s_core_state_values.left.at(defn::CoreStateValue::TotalBoarded) }] = bound_id++;
			accessors.push_back([this]{ return m_history.num_people_boarded; });

			bindings[state_value_id{ defn::s_core_state_values.left.at(defn::CoreStateValue::TotalDelivered) }] = bound_id++;
			accessors.push_back([this]{ return m_history.num_people_delivered; });

			bindings[state_value_id{ defn::s_core_state_values.left.at(defn::CoreStateValue::DistanceCovered) }] = bound_id++;
			accessors.push_back([this]{ return m_history.distance_moved; });

			bindings[state_value_id{ defn::s_core_state_values.left.at(defn::CoreStateValue::TotalQueuingTime) }] = bound_id++;
			accessors.push_back([this]{ return m_history.total_queuing_time; });

			bindings[state_value_id{ defn::s_core_state_values.left.at(defn::CoreStateValue::AvgQueuingTime) }] = bound_id++;
			accessors.push_back([this]
			{
				auto avg_q_time = m_history.total_queuing_time;
				if(m_history.num_people_boarded > 0)
				{
					avg_q_time /= m_history.num_people_boarded;
				}
				return avg_q_time;
			});

			bindings[state_value_id{ defn::s_core_state_values.left.at(defn::CoreStateValue::MaxQueuingTime) }] = bound_id++;
			accessors.push_back([this]{ return m_history.max_queuing_time; });

			bindings[state_value_id{ defn::s_core_state_values.left.at(defn::CoreStateValue::TotalInclusiveQueuingTime) }] = bound_id++;
			accessors.push_back([this]{ return m_history.total_inclusive_queuing_time; });

			bindings[state_value_id{ defn::s_core_state_values.left.at(defn::CoreStateValue::TotalDeliveryTime) }] = bound_id++;
			accessors.push_back([this]{ return m_history.total_delivery_time; });

			bindings[state_value_id{ defn::s_core_state_values.left.at(defn::CoreStateValue::AvgDeliveryTime) }] = bound_id++;
			accessors.push_back([this]
			{
				auto avg_d_time = m_history.total_delivery_time;
				if(m_history.num_people_delivered > 0)
				{
					avg_d_time /= m_history.num_people_delivered;
				}
				return avg_d_time;
			});

			bindings[state_value_id{ defn::s_core_state_values.left.at(defn::CoreStateValue::MaxDeliveryTime) }] = bound_id++;
			accessors.push_back([this]{ return m_history.max_delivery_time; });

			bindings[state_value_id{ defn::s_core_state_values.left.at(defn::CoreStateValue::TotalInclusiveDeliveryTime) }] = bound_id++;
			accessors.push_back([this]{ return m_history.total_inclusive_delivery_time; });

			bindings[state_value_id{ defn::s_core_state_values.left.at(defn::CoreStateValue::TotalIdleTime) }] = bound_id++;
			accessors.push_back([this]{ return m_history.idle_time; });

			for(floor_t f = 0; f < m_num_floors; ++f)
			{
				state_value_id base = elevator_system_defn::floor_name(f);

				bindings[base + defn::s_floor_state_values.left.at(defn::FloorStateValue::NumWaiting)] = bound_id++;
				accessors.push_back([this, f]{ return m_state.floors[f].waiting(); });
				bindings[base + defn::s_floor_state_values.left.at(defn::FloorStateValue::UpPressed)] = bound_id++;
				accessors.push_back([this, f]{ return m_state.floors[f].waiting_up() > 0 ? 1.0 : 0.0; });
				bindings[base + defn::s_floor_state_values.left.at(defn::FloorStateValue::UpPressedDuration)] = bound_id++;
				accessors.push_back([this, f]{ return m_time - m_state.floors[f].buttons[Direction::Up].pressed_time; });
				bindings[base + defn::s_floor_state_values.left.at(defn::FloorStateValue::NumWaitingUp)] = bound_id++;
				accessors.push_back([this, f]{ return m_state.floors[f].waiting_up(); });
				bindings[base + defn::s_floor_state_values.left.at(defn::FloorStateValue::DownPressed)] = bound_id++;
				accessors.push_back([this, f]{ return m_state.floors[f].waiting_down() > 0 ? 1.0 : 0.0; });
				bindings[base + defn::s_floor_state_values.left.at(defn::FloorStateValue::DownPressedDuration)] = bound_id++;
				accessors.push_back([this, f]{ return m_time - m_state.floors[f].buttons[Direction::Down].pressed_time; });
				bindings[base + defn::s_floor_state_values.left.at(defn::FloorStateValue::NumWaitingDown)] = bound_id++;
				accessors.push_back([this, f]{ return m_state.floors[f].waiting_down(); });
				bindings[base + defn::s_floor_state_values.left.at(defn::FloorStateValue::IsDestination)] = bound_id++;
				accessors.push_back([this, f]{ return m_state.passengers_by_dest[f].count > 0 ? 1.0 : 0.0; });
			}

			// TODO: or leave to basic_system???
			auto basic_ag = static_cast<basic_agent*>(m_agent.get());
			basic_ag->initialize_state_value_bindings(bindings, accessors);

			return bindings.size() - initial_count;
		}
	
/*		double elevator_system::get_state_value(state_value_id id) const
		{
			auto idx = m_state_value_ids.at(id);
			return m_state_value_accessors[idx]();
		}
*/

		void elevator_system::concatenate_performance_data(perf_data_t& pd) const
		{
			//		pd["Agents"] += m_agent_update_time;
			//		pd["Box2D"] += m_b2d_update_time;
			//		pd["Observers"] += m_observer_update_time;
		}

	}
}
