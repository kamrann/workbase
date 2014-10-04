// output.h

#ifndef __WB_NN_OUTPUT_H
#define __WB_NN_OUTPUT_H

#include "basic_types.h"

#include <vector>


namespace nnet {

	typedef value_array_t output_array_t;

	class output
	{
	public:
		output(size_t sz): m_values(sz, 0.0)
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

		inline output_array_t::iterator begin()
		{
			return m_values.begin();
		}

		inline output_array_t::iterator end()
		{
			return m_values.end();
		}

		inline output_array_t::const_iterator begin() const
		{
			return m_values.begin();
		}

		inline output_array_t::const_iterator end() const
		{
			return m_values.end();
		}

		inline output_array_t& data()
		{
			return m_values;
		}

	private:
		output_array_t m_values;
	};

}


#endif


