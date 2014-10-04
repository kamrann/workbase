// problem_domain_tbl.h

#ifndef __GADB_PROBLEM_DOMAIN_H
#define __GADB_PROBLEM_DOMAIN_H

#include "genetic_language_tbl.h"

#include <Wt/Dbo/Types>


namespace dbo = Wt::Dbo;

class system_sim;

class problem_domain
{
public:
	typedef dbo::collection< dbo::ptr< genetic_language> >		genetic_language_list;
	
	dbo::ptr< system_sim >		sys_sim;
	genetic_language_list		languages;

	template < class Action >
	void persist(Action& a)
	{
		dbo::belongsTo(a, sys_sim, "problem_sys");

		dbo::hasMany(a, languages, dbo::ManyToOne, "lang_problem");
	}
};


#endif


