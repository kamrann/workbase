// param_constraints.h

#ifndef __WB_PARAM_CONSTRAINTS_H
#define __WB_PARAM_CONSTRAINTS_H

#include "param.h"


namespace prm
{
	struct par_null_constraints
	{

	};

	struct boolean_par_constraints
	{

	};

	struct integer_par_constraints
	{
		int min, max;

		integer_par_constraints():
			min(std::numeric_limits< int >::min()),
			max(std::numeric_limits< int >::max())
		{}
	};

	struct realnum_par_constraints
	{
		double min, max;

		realnum_par_constraints():
			min(-std::numeric_limits< double >::max()),
			max(std::numeric_limits< double >::max())
		{}
	};

	struct enum_par_constraints
	{
		struct val_t
		{
			std::string		name;
			boost::any		val;

			val_t(std::string _n = "", boost::any const& _v = boost::any()):
				name(_n),
				val(_v)
			{}
		};

		std::vector< val_t > vals;

		inline void add(std::string _n, boost::any const& _v)
		{
			vals.push_back(val_t(_n, _v));
		}
	};

	struct string_par_constraints
	{

	};

	struct vector2_par_constraints
	{

	};

	struct vector3_par_constraints
	{

	};


	typedef boost::variant<
		par_null_constraints,
		boolean_par_constraints,
		integer_par_constraints,
		realnum_par_constraints,
		enum_par_constraints,
		string_par_constraints,
		vector2_par_constraints//,
//		vector3_par_constraints
	> par_constraints;
}


#endif


