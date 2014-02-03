// proto_types.h

#ifndef __GK_PROTO_TYPES_H
#define __GK_PROTO_TYPES_H

#include <Eigen/Dense>


typedef float float_t;
typedef Eigen::Matrix< float_t, 3, 1 > vector_t;

/*
#ifdef _DEBUG
class debug_props
{
private:
	static size_t next_id;
	static std::map< const void*, size_t > object_id_map;

public:
	static size_t reg(const void* p)
	{
		std::map< void*, size_t >::iterator it = object_id_map.find(p);
		if(it != object_id_map.end())
		{
			return it->second;
		}
		else
		{
			object_id_map[p] = ++next_id;
			return next_id;
		}
	}

	template < typename Range >
	static void reg_contents(Range const& r)
	{
		for(auto const& e: r)
		{
			reg(&e);
		}
	}
};
#endif
*/

#endif


