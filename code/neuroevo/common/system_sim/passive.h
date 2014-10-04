// passive.h

#ifndef __NE_RTP_PASSIVE_CONRTOLLER_H
#define __NE_RTP_PASSIVE_CONRTOLLER_H

#include "rtp_controller.h"


namespace sys
{
	class passive_controller: public i_controller
	{
	public:
		virtual input_id_list_t get_input_ids() const override
		{
			return input_id_list_t();
		}

		virtual output_list_t process(input_list_t const& inputs) override
		{
			return output_list_t();
		}
	};

}


#endif

