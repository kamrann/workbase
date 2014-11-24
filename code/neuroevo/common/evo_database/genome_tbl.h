// genome_tbl.h

#ifndef __GADB_GENOME_H
#define __GADB_GENOME_H

#include "table_traits.h"

#include <Wt/Dbo/Types>


namespace dbo = Wt::Dbo;

class generation;

class genome
{
public:
	dbo::ptr< generation >			generation;
	std::string						objective_value;	// Stored as YAML, either single real value, or sequence of pareto components
	int								gen_obj_rank;		// Rank (by objective function) of this individual within its generation
	std::vector< unsigned char >	encoding;

	template < class Action >
	void persist(Action& a)
	{
		dbo::field(a, objective_value, "obj_val");
		dbo::field(a, gen_obj_rank, "rank");
		dbo::field(a, encoding, "encoding");

		dbo::belongsTo(a, generation, "genome__generation", dbo::NotNull | dbo::OnDeleteCascade);
	}
};


#endif


