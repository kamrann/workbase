// sys_params_hierarchy.h

#ifndef __NE_SYS_PARAMS_HIERARCHY_H
#define __NE_SYS_PARAMS_HIERARCHY_H

Interface provided by all objects / hierarchy levels (ideally 1-1 mapping between these objects and db tables):
{
	// param_map maps from name string to below variant
//	hdef get_hierarchy_def(param_map)	// need existing param values for determining hierarchy branching
		// (ie. an object which can be specialized (has subtypes) will have a type_id_set (prob string) in its hierarchy def
		// and will query the value of the corresponding param, in order to provide the subhierarchy of the correct subtype
	create_widget(param_map)

	instantiate(param_map)
	write_to_db(param_map)
	read_from_db(param_map)
}


hierarchy_spec	// specifies the structure and behaviour of all possible hierarchies
each hierarchy level should correspond to a db table, and should be able to provide the dbo mapping code
generate_widgets(hierarchy_spec)


variant<
	...		// standard types
	sub		// one type representing sub hierarchy (one to one relation)
	child?	// represents one-many parent/child relation (eg. system_params has potentially multiple agent_params)
> param;




#endif



