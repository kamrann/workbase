// system_update_info.h

#ifndef __NE_BASIC_SYSTEM_UPDATE_INFO_H
#define __NE_BASIC_SYSTEM_UPDATE_INFO_H


namespace sys {

	struct update_info
	{
		enum class Type {
			Realtime,
			Nonrealtime,
		};

		Type type;
		double frequency;	// Updates per second, for realtime systems

		update_info(): type(Type::Nonrealtime), frequency(0.0) {}
	};

}


#endif

