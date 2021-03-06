// generation_tbl.h

#ifndef __GADB_GENERATION_H
#define __GADB_GENERATION_H

#include "table_traits.h"

#include <Wt/Dbo/Types>


namespace dbo = Wt::Dbo;

class evo_run;
class genome;

class generation
{
public:
	typedef dbo::collection< dbo::ptr< genome > >		genome_list;

	dbo::ptr< evo_run >		run;
	int						index;
	double					genotype_diversity;		// TODO: type...?
	genome_list				genomes;

	inline size_t population_size() const
	{
		return genomes.size();
	}

	template < class Action >
	void persist(Action& a)
	{
		dbo::field(a, index, "index");
		dbo::field(a, genotype_diversity, "genotype_diversity");

		dbo::belongsTo(a, run, "generation__evo_run", dbo::NotNull | dbo::OnDeleteCascade);

		dbo::hasMany(a, genomes, dbo::ManyToOne, "genome__generation");
	}
};


#endif


