// elevator_systemstatevalues.h

#ifndef __NE_ELEVATOR_SYSTEM_STATE_VALUES_H
#define __NE_ELEVATOR_SYSTEM_STATE_VALUES_H

#include "elevator_system_defn.h"


namespace sys {
	namespace elev {

		enum class elevator_system_defn::CoreStateValue: unsigned long {
			Time,
			Location,
			Direction,
			Occupancy,
			NumWaiting,
			NumGotOff,
			NumGotOn,

			TotalArrivals,
			TotalBoarded,
			TotalDelivered,
			DistanceCovered,
			TotalQueuingTime,
			AvgQueuingTime,
			MaxQueuingTime,
			TotalInclusiveQueuingTime,
			TotalDeliveryTime,
			AvgDeliveryTime,
			MaxDeliveryTime,
			TotalInclusiveDeliveryTime,
			TotalIdleTime,

			Count,
		};

		enum class elevator_system_defn::FloorStateValue: unsigned long {
			NumWaiting,
			UpPressed,
			UpPressedDuration,
			NumWaitingUp,
			DownPressed,
			DownPressedDuration,
			NumWaitingDown,
			IsDestination,

			Count,
		};

	}
}



#endif

