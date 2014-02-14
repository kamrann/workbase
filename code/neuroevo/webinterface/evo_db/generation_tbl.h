// generation_tbl.h

#ifndef __GADB_GENERATION_H
#define __GADB_GENERATION_H

#include "genome_tbl.h"

#include <Wt/Dbo/Types>


namespace dbo = Wt::Dbo;

class evo_period;

class generation
{
public:
	typedef dbo::collection< dbo::ptr< genome > >		genome_list;

	dbo::ptr< evo_period >	period;
	int						index;
	double					genotype_diversity;		// TODO: type...?
	genome_list				genomes;

	template < class Action >
	void persist(Action& a)
	{
		dbo::field(a, index, "index");
		dbo::field(a, genotype_diversity, "genotype_diversity");

		dbo::belongsTo(a, period, "gen_period");

		dbo::hasMany(a, genomes, dbo::ManyToOne, "geno_gen");
	}
};


#endif


