// rtp_phys_systemstatevalues.h

#ifndef __NE_RTP_PHYS_SYSTEM_STATE_VALUES_H
#define __NE_RTP_PHYS_SYSTEM_STATE_VALUES_H


namespace rtp_phys {

	enum StateValue {
		Time,
		AgentPosX,
		AgentPosY,
		AgentVelX,
		AgentVelY,
		AgentSpeed,
//		AgentAngle,
		AgentKE,

		Count,
	};

	extern std::string const StateValueNames[StateValue::Count];

}


#endif

