// rnn_ev_controller_defn.h

#ifndef __WB_EV_SYS_RNN_EV_CONTROLLER_DEFN_H
#define __WB_EV_SYS_RNN_EV_CONTROLLER_DEFN_H

#include "ev_controller_genetic_mapping.h"
#include "evolved_controller_defn.h"

#include "ddl/ddl.h"
#include "ddl/components/enum_choice.h"


namespace sys {
	namespace ev {

		class rnn_ev_controller_defn
		{
		public:
			rnn_ev_controller_defn(
				evolvable_controller_impl_defn::sys_defn_fn_t& sys_defn_fn,
				evolvable_controller_impl_defn::state_vals_fn_t& sv_fn
				);

			ddl::defn_node get_defn(ddl::specifier& spc);
			std::unique_ptr< i_genetic_mapping > generate(ddl::navigator nav) const;

		public:
			evolvable_controller_impl_defn::sys_defn_fn_t& sys_defn_fn_;
			evolvable_controller_impl_defn::state_vals_fn_t& sv_fn_;

			typedef ga::gene_mutation< double > gene_mut_fn_t;
			ddl::enum_choice< gene_mut_fn_t > gene_mutation_defn_;
		};

	}
}



#endif


