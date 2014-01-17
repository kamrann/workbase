// pareto_multiple_objective.h

#ifndef __PARETO_MULTIPLE_OBJECTIVE_H
#define __PARETO_MULTIPLE_OBJECTIVE_H

#include "multiple_objective.h"
#include "util/pareto_ct.h"

#include <boost/mpl/fold.hpp>
#include <boost/mpl/push_back.hpp>
#include <boost/mpl/vector.hpp>
#include <boost/mpl/for_each.hpp>


namespace mpl = boost::mpl;

template <
	typename TrialData,
	typename ObjFunctions		// MPL list of types deriving from objective_fn
>
class pareto_multiple_objective: public multiple_objective< TrialData, ObjFunctions >
{
private:
	typedef ObjFunctions obj_fns_tl;

	template < typename T >
	struct extract_obj_value_type
	{
		typedef typename T::obj_value_t type;
	};

	typedef typename mpl::fold<
		obj_fns_tl,
		mpl::vector<>,
		mpl::push_back< mpl::_1, extract_obj_value_type< mpl::_2 > >
	>::type obj_values_tl;

public:
	typedef pareto< obj_values_tl > obj_value_t;

private:
	struct eval_ftr
	{
		obj_value_t& v;
		ofdata_t const& ofd;
		TrialData const& td;

		eval_ftr(obj_value_t& _v, ofdata_t const& _ofd, TrialData const& _td): v(_v), ofd(_ofd), td(_td)
		{}

		template < typename T >
		inline void operator() (T t) const
		{
			// Determine the position of T within the typelist
			typedef typename mpl::find< obj_fns_tl, T >::type iter;
			typedef typename mpl::distance< typename mpl::begin< obj_fns_tl >::type, iter > pos;

			fus::at_c< pos::value >(v) = T::evaluate(ofd, td);
		}
	};

public:
	inline obj_value_t evaluate(ofdata_t const& ofd, TrialData const& td) const
	{
		obj_value_t val;
		mpl::for_each< obj_fns_tl >(eval_ftr(val, ofd, td));
		return val;
	}
};


#endif


