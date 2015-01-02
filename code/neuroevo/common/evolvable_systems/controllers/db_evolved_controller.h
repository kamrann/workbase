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
			virtual ddl::defn_node get_defn(ddl::specifier& spc) override;

			virtual controller_ptr create_controller(ddl::navigator nav) const override;

		protected:
			session_fn_t session_fn_;
		};

	}
}


#endif


