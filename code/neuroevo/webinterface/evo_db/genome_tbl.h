// genome_tbl.h

#ifndef __GADB_GENOME_H
#define __GADB_GENOME_H

#include <Wt/Dbo/Types>


namespace dbo = Wt::Dbo;

class generation;

class genome
{
public:
	dbo::ptr< generation >			generation;
	int								gen_obj_rank;	// Rank (by objective function) of this individual within its generation
	std::vector< unsigned char >	encoding;

	template < class Action >
	void persist(Action& a)
	{
		dbo::field(a, gen_obj_rank, "rank");
		dbo::field(a, encoding, "encoding");

		dbo::belongsTo(a, generation, "geno_gen");
	}
};


#endif


