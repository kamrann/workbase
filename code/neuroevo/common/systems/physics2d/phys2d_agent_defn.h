// phys2d_agent_defn.h

#ifndef __NE_PHYS2D_AGENT_DEFN_H
#define __NE_PHYS2D_AGENT_DEFN_H

#include "system_sim/syssim_defs.h"
#include "system_sim/agent_defn.h"

#include "util/fixed_or_random.h"

#include <Box2D/Box2D.h>

#include <random>


namespace sys {
	namespace phys2d {

		class object;

		class phys2d_agent_defn:
			public i_agent_defn
		{
		public:
			virtual std::string update_schema_providor_for_instance(prm::schema::schema_provider_map_handle provider, prm::qualified_path const& prefix) const override;

		protected:
			typedef fixed_or_random< double, std::uniform_real_distribution< double >, rgen_t > random_val_t;

			struct instance_data
			{
				random_val_t pos_x;
				random_val_t pos_y;
				random_val_t orientation;
				random_val_t linvel_angle;
				random_val_t linvel_mag;
				random_val_t angvel;

/*				b2Vec2 pos;
				double orientation;
				b2Vec2 vel;
				double angvel;
*/			};

			static void initialize_instance_data(instance_data& inst, prm::param_accessor acc);
			static void initialize_object_state(object* obj, instance_data const& st, rgen_t& rgen);
		};

	}
}


#endif

