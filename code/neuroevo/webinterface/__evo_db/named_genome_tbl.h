// named_genome_tbl.h

#ifndef __GADB_NAMED_GENOME_H
#define __GADB_NAMED_GENOME_H

#include <Wt/Dbo/Types>


namespace dbo = Wt::Dbo;

class genome;

// TODO: If keep this table, specialise traits so that it uses the genome foreign key as its primary key
class named_genome
{
public:
	dbo::ptr< genome >		gn;
	std::string				name;

	template < class Action >
	void persist(Action& a)
	{
		dbo::field(a, name, "name");

		dbo::belongsTo(a, gn, "named_genome__genome", dbo::NotNull | dbo::OnDeleteCascade);
	}
};


#endif


