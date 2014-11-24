// db_evolved_controller.h

#ifndef __WB_EV_SYS_DB_EVOLVED_CONTROLLER_H
#define __WB_EV_SYS_DB_EVOLVED_CONTROLLER_H

#include "system_sim/controller_defn.h"


class evodb_session;

namespace sys {
	namespace ev {

		class db_evolved_controller_defn:
			public i_controller_defn
		{
		public:
			typedef std::function< std::unique_ptr< evodb_session >() > session_fn_t;
			
		public:
			db_evolved_controller_defn(session_fn_t session_fn);

		public:
			virtual std::string get_name() const override;
			virtual std::string update_schema_providor(prm::schema::schema_provider_map_handle provider, prm::qualified_path const& prefix) const override;

			virtual controller_ptr create_controller(prm::param_accessor acc) const override;

		protected:
			session_fn_t session_fn_;
		};

	}
}


#endif


