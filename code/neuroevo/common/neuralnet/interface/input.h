// input.h

#ifndef __WB_NN_INPUT_H
#define __WB_NN_INPUT_H

#include "basic_types.h"

#include <vector>


namespace nnet {

	typedef value_array_t input_array_t;

	class input
	{
	public:
		input(size_t sz): m_values(sz, 0.0)
		{}
		input(value_t* vals, size_t sz): m_values{ vals, vals + sz }
		{}
		input(input_array_t const& in): m_values(std::move(in))
		{}
		input(input_array_t&& in): m_values(std::move(in))
		{}

	public:
		inline size_t size() const
		{
			return m_values.size();
		}

		inline value_t operator[] (const int i) const
		{
			return m_values[i];
		}

		inline value_t& operator[] (const int i)
		{
			return m_values[i];
		}

	private:
		input_array_t m_values;
	};

}


#endif


