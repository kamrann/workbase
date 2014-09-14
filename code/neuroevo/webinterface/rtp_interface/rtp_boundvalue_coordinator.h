// rtp_boundvalue_coordinator.h

#ifndef __NE_RTP_BOUND_VALUE_COORDINATOR_H
#define __NE_RTP_BOUND_VALUE_COORDINATOR_H


namespace rtp {

	class i_boundvalue_provider;
	class i_boundvalue_consumer;

	class boundvalue_coordinator
	{
	public:
		void set_provider(i_boundvalue_provider* provider);
		void add_consumer(i_boundvalue_consumer* consumer);
		
		void initialize();
	};

}


#endif


