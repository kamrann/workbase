// system_sim_tbl.h

#ifndef __GADB_SYSTEM_SIM_H
#define __GADB_SYSTEM_SIM_H

#include "problem_domain_tbl.h"

#include <Wt/Dbo/Types>


namespace dbo = Wt::Dbo;

class system_sim
{
public:
	typedef dbo::collection< dbo::ptr< problem_domain > >		problem_domain_list;
	
	problem_domain_list		problems;

	template < class Action >
	void persist(Action& a)
	{
		dbo::hasMany(a, problems, dbo::ManyToOne, "problem_sys");
	}
};


#endif


