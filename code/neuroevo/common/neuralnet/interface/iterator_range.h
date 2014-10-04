// iterator_range.h

#ifndef __WB_NN_ITERATOR_RANGE_H
#define __WB_NN_ITERATOR_RANGE_H

#include "iterator.h"


namespace nnet {

	template < typename IteratorType >
	class iterator_range
	{
	public:
		typedef IteratorType iterator_type;

	public:
		inline size_t size() const
		{
			return m_size;
		}

		inline iterator_type begin() const
		{
			return m_first;
		}

		inline iterator_type end() const
		{
			return m_last;
		}

	public:
		iterator_range(iterator_type first, iterator_type last, size_t size): m_first(first), m_last(last), m_size(size)
		{}

	private:
		size_t m_size;
		iterator_type m_first, m_last;
	};


	typedef iterator_range< neuron_iterator > neuron_range;
	typedef iterator_range< connection_iterator > connection_range;

}


#endif


