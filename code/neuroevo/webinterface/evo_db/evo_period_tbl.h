// evo_period_tbl.h

#ifndef __GADB_EVO_PERIOD_H
#define __GADB_EVO_PERIOD_H

#include "generation_tbl.h"

#include <Wt/Dbo/Types>


namespace dbo = Wt::Dbo;

class genetic_language;

class evo_period
{
public:
	typedef dbo::collection< dbo::ptr< generation > >		generation_list;
	
	dbo::ptr< genetic_language >	language;
	int								initial_pop_size;
	generation_list					generations;

	template < class Action >
	void persist(Action& a)
	{
		dbo::field(a, initial_pop_size, "initial_pop_size");

		dbo::belongsTo(a, language, "period_lang");

		dbo::hasMany(a, generations, dbo::ManyToOne, "gen_period");
	}
};


#endif


