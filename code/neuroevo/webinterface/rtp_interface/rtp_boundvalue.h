// rtp_boundvalue.h

#ifndef __NE_RTP_BOUND_VALUE_H
#define __NE_RTP_BOUND_VALUE_H

#include <vector>
#include <string>


namespace rtp {

	typedef double boundvalue;

	typedef std::string unbound_id;
//	typedef unsigned long boundvalue_id;

	typedef std::vector< unbound_id > unbound_id_set;
//	typedef std::vector< boundvalue_id > boundvalue_id_set;

	typedef std::vector< boundvalue > boundvalue_buffer;

	class boundvalue_accessor
	{
	public:
		inline boundvalue const& get() const
		{
			return *m_ptr;
		}

	public:
		boundvalue_accessor(boundvalue const* ptr): m_ptr(ptr)
		{}

	private:
		boundvalue const* const m_ptr;
	};

	class boundvalue_set_accessor
	{
	public:
		inline boundvalue const& operator[](int index) const
		{
			return m_accessors[index].get();
		}

//		boundvalue_set_accessor()

	private:
		std::vector< boundvalue_accessor > m_accessors;
	};

}


#endif


