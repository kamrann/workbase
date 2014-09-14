// rtp_dumb_elevator_controller.cpp

#include "rtp_dumb_elevator_controller.h"


namespace rtp
{
	
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

