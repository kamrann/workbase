// input.h

#ifndef __WB_NN_INPUT_H
#define __WB_NN_INPUT_H

#include "basic_types.h"

#include <vector>


namespace nnet {

	template < typename InputValue >
	class input_t
	{
	public:
		typedef InputValue input_value_t;
		typedef std::vector< input_value_t > input_list_t;
		typedef typename input_list_t::iterator iterator;

	public:
		input_t(size_t sz): m_values(sz, 0.0)
		{}
		input_t(input_value_t* vals, size_t sz): m_values{ vals, vals + sz }
		{}
		input_t(input_list_t const& in): m_values(in)
		{}
		input_t(input_list_t&& in): m_values(std::move(in))
		{}
		input_t(iterator first, iterator last): m_values(first, last)
		{}

	public:
		inline size_t size() const
		{
			return m_values.size();
		}

		inline input_value_t operator[] (const int i) const
		{
			return m_values[i];
		}

		inline input_value_t& operator[] (const int i)
		{
			return m_values[i];
		}

		iterator begin()
		{
			return m_values.begin();
		}

		iterator end()
		{
			return m_values.end();
		}

	private:
		input_list_t m_values;
	};

	typedef input_t< value_t > input;
	typedef input::input_list_t input_array_t;

	typedef input_t< range_t > input_range;	// TODO: Will likely need separate class if want to deal with contrained inputs
	typedef input_range::input_list_t input_range_array_t;

}


#endif


