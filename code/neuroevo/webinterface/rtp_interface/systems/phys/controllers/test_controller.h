// test_controller.h

#ifndef __NE_RTP_PHYS_TEST_CONRTOLLER_H
#define __NE_RTP_PHYS_TEST_CONRTOLLER_H

#include "../rtp_phys_controller.h"
#include "../rtp_phys_system.h"

//
#include "../bodies/test_body.h"
//

#include <vector>


namespace rtp
{
	class test_controller:
		public i_phys_controller
	{
	public:
		test_controller()
		{}

		virtual input_id_list_t get_input_ids() const
		{
			return input_id_list_t();
		}

		virtual output_list_t process(input_list_t const& inputs)
		{
			return output_list_t();
		}
	};

}


#endif

