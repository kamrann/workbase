// transform_hierarchy.h

#ifndef __GLES_UTIL_TRANSFORM_HIERARCHY_H
#define __GLES_UTIL_TRANSFORM_HIERARCHY_H

#include "util/gtree/generic_tree.hpp"

#include <Eigen/Dense>

#include <list>


// TEMP
template < typename FloatType >
struct xf_hier_node_attribs_sel
{
	typedef Eigen::Transform< FloatType, 3, Eigen::Affine > type;
};

struct xf_hier_edge_attribs
{};

template <
//	typename LevelData,
	typename FloatType = float
>
class transform_hierarchy:
	public wb::gtree::generic_tree< typename xf_hier_node_attribs_sel< FloatType >::type, xf_hier_edge_attribs >
{
public:
	typedef FloatType float_t;
	typedef //typename xf_hier_node_attribs_sel< float_t >::type
		node_attribs_t xform_t;
//	typedef LevelData level_data_t;
//	typedef Eigen::Transform< float_t, 3, Eigen::Affine > xform_t;
//	typedef int level_id;

private:
//	xform_t xform;
//	level_data_t data;
//	transform_hierarchy* parent;
//	std::list< transform_hierarchy > children;

public:
	transform_hierarchy()//: xform(xform_t::Identity()), parent(nullptr)
	{}

public:
	xform_t get_transform(node_descriptor n) const
	{
		node_descriptor p;
		bool has_parent;
		boost::tie(p, has_parent) = get_parent(n);
		if(!has_parent)
		{
			return get_local_transform(n);
		}
		else
		{
			return get_parent_transform(n) * get_local_transform(n);
		}
	}

	xform_t get_local_transform(node_descriptor n) const
	{
		return (*this)[n];
	}

	xform_t get_parent_transform(node_descriptor n) const
	{
		node_descriptor p;
		bool has_parent;
		boost::tie(p, has_parent) = get_parent(n);
		assert(has_parent);
		return get_transform(p);
	}

public:

};


#endif



