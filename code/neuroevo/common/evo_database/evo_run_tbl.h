// evo_run_tbl.h

#ifndef __GADB_EVO_RUN_H
#define __GADB_EVO_RUN_H

#include "table_traits.h"
#include "custom_fields.h"

#include <Wt/Dbo/Types>

#include <chrono>


namespace dbo = Wt::Dbo;

class generation;

class evo_run
{
public:
	typedef dbo::collection< dbo::ptr< generation > >		generation_list;
	
	std::string								params;		// YAML
	std::chrono::system_clock::time_point	started;
	std::chrono::system_clock::time_point	ended;
	generation_list							generations;

	inline size_t num_generations() const
	{
		return generations.size();
	}

	template < class Action >
	void persist(Action& a)
	{
		dbo::field(a, params, "params");
		dbo::field(a, started, "started");
		dbo::field(a, ended, "ended");

		dbo::hasMany(a, generations, dbo::ManyToOne, "generation__evo_run");
	}
};


#endif


