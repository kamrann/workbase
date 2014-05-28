// evo_run_tbl.h

#ifndef __GADB_EVO_RUN_H
#define __GADB_EVO_RUN_H

#include "table_traits.h"

#include <Wt/Dbo/Types>

#include <boost/date_time/posix_time/posix_time.hpp>


namespace dbo = Wt::Dbo;

class sys_params;
class evo_params;
class generation;

class evo_run
{
public:
	typedef dbo::collection< dbo::ptr< generation > >		generation_list;
	
	dbo::weak_ptr< sys_params >		system_params;
	dbo::weak_ptr< evo_params >		evolution_params;
	boost::posix_time::ptime		started;
	boost::posix_time::ptime		ended;
	generation_list					generations;

	inline size_t num_generations() const
	{
		return generations.size();
	}

	template < class Action >
	void persist(Action& a)
	{
		dbo::field(a, started, "started");
		dbo::field(a, ended, "ended");

//		dbo::belongsTo(a, language, "period_lang");

		dbo::hasOne(a, system_params, "evo_run");
		dbo::hasOne(a, evolution_params, "evo_run");
		dbo::hasMany(a, generations, dbo::ManyToOne, "generation__evo_run");
	}
};


#endif


