// composite_body_defn.h

#ifndef __NE_RTP_PHYS2D_COMPOSITE_BODY_DEFN_H
#define __NE_RTP_PHYS2D_COMPOSITE_BODY_DEFN_H

//#include "phys2d_agent_defn.h"
#include "system_sim/system_state_values.h"

#include <string>
#include <vector>


namespace sys {
	namespace phys2d {

		class composite_body_defn//:
//			public phys2d_agent_defn
		{
		public:
			virtual state_value_id_list get_state_values() const;

		public:
//			void add_component_body(b2Body* comp, boost::any&& data);
			void add_revolute(std::string name);
			void clear_revolutes();

		protected:
			std::vector< std::string > m_revolutes;
		};

	}
}



#endif

