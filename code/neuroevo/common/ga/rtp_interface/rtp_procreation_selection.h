// rtp_procreation_selection.h

#ifndef __GA_RTP_PROCREATION_SELECTION_H
#define __GA_RTP_PROCREATION_SELECTION_H

#include "rtp_individual.h"

#include <vector>


class i_procreation_selection
{
public:
	virtual void preprocess_population(
		std::vector< rtp_individual >::const_iterator pop_start,
		std::vector< rtp_individual >::const_iterator pop_end
		) = 0;

	virtual void select_parents(
		size_t num_parents,
		std::vector< rtp_individual >::const_iterator pop_start,
		std::vector< rtp_individual >::const_iterator pop_end,
		std::vector< rtp_genome_wrapper const* >::iterator dest
		) = 0;
};


template < typename Impl >
class proc_sel_wrapper: public i_procreation_selection
{
public:
	inline proc_sel_wrapper(rgen_t& rgen): m_impl(rgen)
	{}

	virtual void preprocess_population(
		std::vector< rtp_individual >::const_iterator pop_start,
		std::vector< rtp_individual >::const_iterator pop_end
		)
	{
		m_impl.preprocess_population(pop_start, pop_end);
	}

	virtual void select_parents(
		size_t num_parents,
		std::vector< rtp_individual >::const_iterator pop_start,
		std::vector< rtp_individual >::const_iterator pop_end,
		std::vector< rtp_genome_wrapper const* >::iterator dest
		)
	{
		m_impl.select_parents(num_parents, pop_start, pop_end, dest);
	}

private:
	Impl m_impl;
};



#endif


