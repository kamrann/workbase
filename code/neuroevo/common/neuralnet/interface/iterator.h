// iterator.h

#ifndef __WB_NN_ITERATOR_H
#define __WB_NN_ITERATOR_H

#include "neuron.h"
#include "connection.h"

#include <iterator>
#include <vector>
#include <memory>


namespace nnet {

	template< typename ValueType >
	class iterator:
		public std::iterator< std::forward_iterator_tag, const ValueType >
	{
	public:
		typedef std::vector< value_type > data_array_t;
		typedef typename data_array_t::const_iterator data_iterator_t;
		typedef std::shared_ptr< data_array_t > data_array_ptr_t;

	public:
		iterator(): m_data(nullptr), m_pos()
		{}

		iterator(data_array_t&& data):
			m_data(std::make_shared< data_array_t >(std::move(data))),
			m_pos(m_data->begin())
		{}

		iterator(data_array_ptr_t ptr):
			m_data(ptr),
			m_pos(ptr->begin())
		{}

	public:
		iterator& operator++ ()
		{
			++m_pos;
			return *this;
		}

		iterator operator++ (int)
		{
			iterator tmp(*this);
			operator++();
			return tmp;
		}

		bool operator== (const iterator& rhs)
		{
			auto const this_end = (m_pos._Getcont() == nullptr || m_pos == m_data->end());
			auto const rhs_end = (rhs.m_pos._Getcont() == nullptr || rhs.m_pos == m_data->end());
			return
				this_end && rhs_end ||
				!this_end && !rhs_end && m_pos == rhs.m_pos;
		}
		
		bool operator!= (const iterator& rhs)
		{
			return !operator==(rhs);
		}

		value_type const& operator* ()
		{
			return *m_pos;
		}

	private:
		data_array_ptr_t m_data;
		data_iterator_t m_pos;
	};


	typedef iterator< neuron_data > neuron_iterator;
	typedef iterator< connection_data > connection_iterator;

}


#endif


