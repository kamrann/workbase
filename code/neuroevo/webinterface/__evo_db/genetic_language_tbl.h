// genetic_language_tbl.h

#ifndef __GADB_GENETIC_LANGUAGE_H
#define __GADB_GENETIC_LANGUAGE_H

#include "evo_period_tbl.h"

#include "ga/genetic_language.h"

#include <Wt/Dbo/Types>


namespace dbo = Wt::Dbo;

class problem_domain;

class genetic_language
{
public:
	typedef dbo::collection< dbo::ptr< evo_period > >		evo_period_list;
	
	dbo::ptr< problem_domain >		problem;
	ga::GeneticLanguageEnum			lang;
	evo_period_list					periods;

	template < class Action >
	void persist(Action& a)
	{
		dbo::field(a, lang, "language");

		dbo::belongsTo(a, problem, "lang_problem");

		dbo::hasMany(a, periods, dbo::ManyToOne, "period_lang");
	}
};


#endif


