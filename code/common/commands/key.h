// key.h

#ifndef __WB_KEY_H
#define __WB_KEY_H


namespace wb {
	namespace cmd {

		struct key
		{
			enum class KeyCode {
				Up,
				Down,
			};

			KeyCode code;
		};

	}
}


#endif


