// rtp_genome_wrap.h

#ifndef __GA_RTP_GENOME_WRAP_H
#define __GA_RTP_GENOME_WRAP_H

// TODO: rearrange
#include "../../../webinterface/rtp_interface/rtp_genome.h"
//

#include <boost/shared_ptr.hpp>


//class i_genome;

class rtp_genome_wrapper
{
public:
	rtp_genome_wrapper(boost::shared_ptr< i_genome > gn = nullptr): m_gn(gn ? gn->clone() : nullptr)
	{

	}

	rtp_genome_wrapper(rtp_genome_wrapper const& other): m_gn(other.m_gn->clone())
	{
		
	}

	inline rtp_genome_wrapper& operator= (rtp_genome_wrapper const& rhs)
	{
		m_gn.reset(rhs.m_gn->clone());
		return *this;
	}

	inline boost::shared_ptr< i_genome > get() const
	{
		return m_gn;
	}

	template < typename T >	// T should derive from i_genome
	inline T& cast_to()
	{
		return *boost::static_pointer_cast<T>(m_gn);
	}

	template < typename T >	// T should derive from i_genome
	inline T const& cast_to() const
	{
		return *boost::static_pointer_cast<T const>(m_gn);
	}

protected:
	boost::shared_ptr< i_genome >	m_gn;
};


template < typename T >
inline T& rtp_cast(rtp_genome_wrapper& gn)
{
	return gn.cast_to< T >();
}

template < typename T >
inline T const& rtp_cast(rtp_genome_wrapper const& gn)
{
	return gn.cast_to< T >();
}


#endif


