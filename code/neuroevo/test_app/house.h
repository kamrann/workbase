// house.h

#ifndef __HOUSE_H
#define __HOUSE_H

#include "params.h"


class House: public hierarchy_object
{
public:
	struct record
	{
		std::string		m_location;

		template < class Action >
		void persist(Action& a)
		{
			dbo::field(a, m_location, "name");

			dbo::belongsTo(a, dbo::ptr< Animal::record >, dbo::NotNull);
		}
	};

public:
	virtual std::string hierarchy_level_name()
	{
		return "House";
	}

	virtual param_spec_list get_specs()
	{
		param_spec_list sl;
		sl.push_back(param_spec(ParamType::String, prm::string_par_constraints(), "Location"));
		return sl;
	}
};



#endif


