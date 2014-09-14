// rtp_dumb_elevator_controller.h

#ifndef __NE_RTP_DUMB_ELEVATOR_CONTROLLER_H
#define __NE_RTP_DUMB_ELEVATOR_CONTROLLER_H

#include "../rtp_controller.h"


namespace rtp
{
	class dumb_elevator_controller: public i_controller
	{
	public:
		virtual input_id_list_t get_input_ids() const override;
		virtual output_list_t process(input_list_t const& inputs) override;
	};

}


#endif

