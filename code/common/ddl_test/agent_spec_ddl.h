// agent_spec_ddl.h

#ifndef __AGENT_SPEC_DDL_H
#define __AGENT_SPEC_DDL_H

#include "ddl/ddl.h"

//#include "agent_spec_ddl.h"


using namespace ddl;


struct agent_spec_schema
{
	defn_node get_defn(specifier& spc)
	{
		defn_node spec_type = spc.enumeration()
			(define_enum_fixed{}("biped")("quadruped"))
			;

		return spc.composite()(define_children{}
			("spec_type", spec_type)
			);
	}
};


#endif


