// table_traits.h

#ifndef __GADB_TABLE_TRAITS_H
#define __GADB_TABLE_TRAITS_H

#if 0

#include "../rtp_interface/systems/rtp_system.h"	// For SystemType
#include "../rtp_interface/systems/phys/rtp_phys_agent_body_spec.h"
#include "../rtp_interface/systems/phys/rtp_phys_evolvable_controller.h"
#include "../rtp_interface/systems/phys/controllers/mlp.h"

#include <Wt/Dbo/Types>


class evo_run;
class evo_params;
class sys_params;
class sys_type;
class phys_sys_params;
class phys_agent_params;
class phys_agent_type;
class phys_controller_params;
class phys_controller_type;
class phys_mlp_controller_params;
class phys_mlp_inputs_type;
class nac_sys_params;

namespace Wt { namespace Dbo {

	template<>
	struct dbo_traits<evo_params>: public dbo_default_traits
	{
		typedef ptr<evo_run> IdType;

		static IdType invalidId();

		static const char *surrogateIdField() { return 0; }
	};

	template<>
	struct dbo_traits<sys_params>: public dbo_default_traits
	{
		typedef ptr<evo_run> IdType;

		static IdType invalidId();

		static const char *surrogateIdField() { return 0; }
	};

	template<>
	struct dbo_traits<sys_type>: public dbo_default_traits
	{
		typedef SystemType IdType;

		static IdType invalidId();

		static const char *surrogateIdField() { return 0; }
	};

	template<>
	struct dbo_traits<phys_sys_params>: public dbo_default_traits
	{
		typedef ptr<sys_params> IdType;

		static IdType invalidId();

		static const char *surrogateIdField() { return 0; }
	};

	template<>
	struct dbo_traits<phys_agent_params>: public dbo_default_traits
	{
		typedef ptr<phys_sys_params> IdType;

		static IdType invalidId();

		static const char *surrogateIdField() { return 0; }
	};

	template<>
	struct dbo_traits<phys_agent_type>: public dbo_default_traits
	{
		typedef rtp::agent_body_spec::Type IdType;

		static IdType invalidId();

		static const char *surrogateIdField() { return 0; }
	};

	template<>
	struct dbo_traits<phys_controller_params>: public dbo_default_traits
	{
		typedef ptr<phys_sys_params> IdType;

		static IdType invalidId();

		static const char *surrogateIdField() { return 0; }
	};

	template<>
	struct dbo_traits<phys_controller_type>: public dbo_default_traits
	{
		typedef rtp::evolvable_controller::Type IdType;

		static IdType invalidId();

		static const char *surrogateIdField() { return 0; }
	};

	template<>
	struct dbo_traits<phys_mlp_controller_params>: public dbo_default_traits
	{
		typedef ptr<phys_controller_params> IdType;

		static IdType invalidId();

		static const char *surrogateIdField() { return 0; }
	};

	template<>
	struct dbo_traits<phys_mlp_inputs_type>: public dbo_default_traits
	{
		typedef rtp::mlp_controller::Type IdType;

		static IdType invalidId();

		static const char *surrogateIdField() { return 0; }
	};

	template<>
	struct dbo_traits<nac_sys_params>: public dbo_default_traits
	{
		typedef ptr<sys_params> IdType;

		static IdType invalidId();

		static const char *surrogateIdField() { return 0; }
	};

}
}

#endif

#endif


