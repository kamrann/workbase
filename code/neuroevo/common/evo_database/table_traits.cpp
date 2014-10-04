// table_traits.cpp

#include "table_traits.h"

#if 0

#include "evo_run_tbl.h"
#include "sys_params_tbl.h"
#include "sys_type_tbl.h"
#include "phys_sys_params_tbl.h"
#include "phys_agent_params_tbl.h"
#include "phys_agent_type_tbl.h"
#include "phys_controller_params_tbl.h"
#include "phys_controller_type_tbl.h"
#include "phys_mlp_controller_params_tbl.h"
#include "phys_mlp_inputs_type_tbl.h"
#include "nac_sys_params_tbl.h"
#include "evo_params_tbl.h"
#include "generation_tbl.h"
#include "genome_tbl.h"


namespace Wt { namespace Dbo {

//	template<>
	dbo_traits<evo_params>::IdType dbo_traits<evo_params>::invalidId()
	{
		return ptr<evo_run>();
	}

//	template<>
	dbo_traits<sys_params>::IdType dbo_traits<sys_params>::invalidId()
	{
		return ptr<evo_run>();
	}

	dbo_traits<sys_type>::IdType dbo_traits<sys_type>::invalidId()
	{
		return SystemType::None;
	}

//	template<>
	dbo_traits<phys_sys_params>::IdType dbo_traits<phys_sys_params>::invalidId()
	{
		return ptr<sys_params>();
	}

	dbo_traits<phys_agent_params>::IdType dbo_traits<phys_agent_params>::invalidId()
	{
		return ptr<phys_sys_params>();
	}

	dbo_traits<phys_agent_type>::IdType dbo_traits<phys_agent_type>::invalidId()
	{
		return rtp::agent_body_spec::Type::None;
	}

	dbo_traits<phys_controller_params>::IdType dbo_traits<phys_controller_params>::invalidId()
	{
		return ptr<phys_sys_params>();
	}

	dbo_traits<phys_controller_type>::IdType dbo_traits<phys_controller_type>::invalidId()
	{
		return rtp::evolvable_controller::Type::None;
	}

	dbo_traits<phys_mlp_controller_params>::IdType dbo_traits<phys_mlp_controller_params>::invalidId()
	{
		return ptr<phys_controller_params>();
	}

	dbo_traits<phys_mlp_inputs_type>::IdType dbo_traits<phys_mlp_inputs_type>::invalidId()
	{
		return rtp::mlp_controller::Type::None;
	}

//	template<>
	dbo_traits<nac_sys_params>::IdType dbo_traits<nac_sys_params>::invalidId()
	{
		return ptr<sys_params>();
	}


	template struct dbo_traits< evo_params >;
	template struct	dbo_traits< sys_params >;
	template struct	dbo_traits< sys_type >;
	template struct	dbo_traits< phys_sys_params >;
	template struct	dbo_traits< phys_agent_params >;
	template struct	dbo_traits< phys_agent_type >;
	template struct	dbo_traits< phys_controller_params >;
	template struct	dbo_traits< phys_controller_type >;
	template struct	dbo_traits< phys_mlp_controller_params >;
	template struct	dbo_traits< phys_mlp_inputs_type >;
	template struct	dbo_traits< nac_sys_params >;

}
}

#endif



