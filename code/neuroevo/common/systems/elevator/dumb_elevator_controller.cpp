// dumb_elevator_controller.cpp

#include "dumb_elevator_controller.h"

#include "params/param_accessor.h"
#include "params/schema_builder.h"


namespace sys {
	namespace elev {

		std::string dumb_elevator_controller_defn::get_name() const
		{
			return "dumb_controller";
		}

		namespace sb = prm::schema;

		std::string dumb_elevator_controller_defn::update_schema_providor(prm::schema::schema_provider_map_handle provider, prm::qualified_path const& prefix) const
		{
			auto relative = get_name();
			auto path = prefix + relative;

			(*provider)[path] = [=](prm::param_accessor acc)
			{
				auto s = sb::list(relative);
				return s;
			};

			return relative;
		}

		controller_ptr dumb_elevator_controller_defn::create_controller() const
		{
			return std::make_unique< dumb_elevator_controller >();
		}


		i_controller::input_id_list_t dumb_elevator_controller::get_input_ids() const
		{
			return input_id_list_t{};
		}

		i_controller::output_list_t dumb_elevator_controller::process(input_list_t const& inputs)
		{
			output_list_t output{};
			/*
			if(m_state.is_any_floor_request())
			{
			if(m_state.at_top())
			{
			dec.direction = Direction::Down;
			}
			else if(m_state.at_bottom())
			{
			dec.direction = Direction::Up;
			}
			else
			{
			dec.direction = (Direction)std::uniform_int_distribution<>(0, 1)(m_rgen);
			}
			}
			else
			{
			dec.direction = Direction::None;
			}

			dec.stop =
			m_state.passengers_by_dest[m_state.location].count > 0 ||
			(
			is_moving(dec.direction) &&
			!m_state.floors[m_state.location].queues[dec.direction].empty()
			);
			*/
			return output;
		}

	}
}

