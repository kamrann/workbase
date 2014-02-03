// agent.h

#ifndef __GK_AGENT_H
#define __GK_AGENT_H

#include "graph_defs/graph_defn.h"
#include "util/graphics3d/transform_hierarchy.h"
#include "util/orientation.h"

#include <boost/shared_ptr.hpp>


class agent
{
public:
	bool at_junction;						// If true, agent is within a node junction, otherwise agent is on a connection.
	boost::optional< gk::g_edge > e1;		// If at_junction, (optional) incident edge to junction; otherwise, edge along which currently travelling.
	gk::g_node n;							// If at_junction, junction node; otherwise unused.
	boost::optional< gk::g_edge > e2;		// If at_junction, (optional) outgoing edge from junction; otherwise unused.

	float pos;					// Point along connection or junction spline. [0, 1)

	// TODO: Maybe store elsewhere?
	Eigen::Vector3f up;

	float outer_rot_speed;		// Rotational speed of outer section
	float energy_level;			// Agent energy level

	// Transform hierarchy
	typedef transform_hierarchy< float > xf_hier_t;
	boost::shared_ptr< xf_hier_t > xform_hier;

	typedef xf_hier_t::node_descriptor xf_loc_t;
	xf_loc_t base_xf, outer_xf, inner_xf;

	agent(): at_junction(true), pos(0.0f), outer_rot_speed(0.0f), energy_level(0.0f)
	{

	}

	void init()
	{
/*		Eigen::Quaternionf base = base_orientation(
			Eigen::Vector3f(0.0f, 1.0f, 0.0f),
			Eigen::Vector3f(0.0f, 0.0f, -1.0f)
			);
*/		xform_hier.reset(new xf_hier_t);
		base_xf = xform_hier->create_root(xf_hier_t::xform_t::Identity()).first;
			//xf_hier_t::xform_t(base.matrix())).first;
		outer_xf = xform_hier->add_node(base_xf, xf_hier_t::edge_attribs_t(), xf_hier_t::xform_t::Identity()).second;
		inner_xf = xform_hier->add_node(base_xf, xf_hier_t::edge_attribs_t(), xf_hier_t::xform_t::Identity()).second;
	}
};


#endif

