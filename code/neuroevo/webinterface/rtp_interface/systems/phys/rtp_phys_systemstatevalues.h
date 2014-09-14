// rtp_phys_systemstatevalues.h

#ifndef __NE_RTP_PHYS_SYSTEM_STATE_VALUES_H
#define __NE_RTP_PHYS_SYSTEM_STATE_VALUES_H


namespace rtp {

	struct _StateValue
	{
		enum StateValue {
			Time,

			Count,
		};
	};
	typedef _StateValue::StateValue StateValue;

	extern std::string const StateValueNames[StateValue::Count];

}


#endif

