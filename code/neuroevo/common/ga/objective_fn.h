// objective_fn.h

#ifndef __OBJECTIVE_FUNCTION_H
#define __OBJECTIVE_FUNCTION_H

#include "util/meta_dependencies.h"

#include <boost/mpl/inherit.hpp>
#include <boost/mpl/inherit_linearly.hpp>
#include <boost/mpl/for_each.hpp>


// Class from which all objective functions should be derived
struct objective_fn: public meta_util::has_dependencies_base
{};


// Structure embedding all data components required by the objective function(s), and providing composite update() and finalise() methods.
template <
	typename OFD_Components		// MPL typelist of objective function data components
>
struct ofdata: public mpl::inherit_linearly<
	OFD_Components,
	mpl::inherit< mpl::_1, mpl::_2 >
>::type
{
	typedef OFD_Components ofd_components_tl;

	template < typename agent_decision, typename agent_state, typename envt_state >
	struct update_ftr
	{
		agent_decision const& dec;
		agent_state const& agent_st;
		envt_state const& envt_st;
		ofdata& ofd;

		update_ftr(agent_decision const& _dec, agent_state const& _agent_st, envt_state const& _envt_st, ofdata& _ofd):
			dec(_dec), agent_st(_agent_st), envt_st(_envt_st), ofd(_ofd)
		{}

		template < typename T >
		inline void operator() (T const&)
		{
			T::update(dec, agent_st, envt_st, ofd);
		}
	};

	struct finalise_ftr
	{
		ofdata& ofd;

		finalise_ftr(ofdata& _ofd):
			ofd(_ofd)
		{}

		template < typename T >
		inline void operator() (T const&)
		{
			T::finalise(ofd);
		}
	};

	template < typename agent_decision, typename agent_state, typename envt_state >
	inline void update(agent_decision const& dec, agent_state const& agent_st, envt_state const& envt_st)
	{
		mpl::for_each< ofd_components_tl >(update_ftr< agent_decision, agent_state, envt_state >(dec, agent_st, envt_st, *this));
	}

	inline void finalise()
	{
		mpl::for_each< ofd_components_tl >(finalise_ftr(*this));
	}
};


#endif


