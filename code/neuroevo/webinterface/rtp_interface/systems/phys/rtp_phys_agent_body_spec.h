// rtp_phys_agent_body_spec.h

#ifndef __NE_RTP_PHYS_AGENT_BODY_SPEC_H
#define __NE_RTP_PHYS_AGENT_BODY_SPEC_H

#include "../../rtp_defs.h"
#include "../rtp_agent.h"

#include "wt_param_widgets/pw_fwd.h"

#include "util/fixed_or_random.h"

#include <boost/random/uniform_real_distribution.hpp>

#include <string>


class b2World;

namespace YAML {
	class Node;
}

namespace rtp {

	class agent_body;

	class agent_body_spec
	{
	public:
		enum Type {
			TestCreature,
			TestQuadruped,
			MinimalBiped,
			TestBiped,
			Spaceship,

			Count,
			None = Count,
			Default = None,
		};

		static std::string const Names[Type::Count];

		static std::string update_schema_providor(prm::schema::schema_provider_map_handle provider, prm::qualified_path const& prefix);
		static void create_base_instance(prm::param_accessor param, agent_body_spec* s);
		static agent_body_spec* create_instance(prm::param_accessor param);
		static void create_base_instance(prm::param& param, agent_body_spec* s);

		static agent_sensor_name_list sensor_inputs(Type type);
		static size_t num_effectors(Type type);

	protected:
		agent_body_spec(Type t);

	public:
		Type m_type;

		// TODO: No longer really sure what exactly the spec thing is for, but if for some kind of instancing, then
		// probably initial position/velocity params shouldn't be part of it, since can be different per instance??
		fixed_or_random< double, boost::random::uniform_real_distribution< double >, rgen_t > m_initial_pos_x;
		fixed_or_random< double, boost::random::uniform_real_distribution< double >, rgen_t > m_initial_pos_y;
		fixed_or_random< double, boost::random::uniform_real_distribution< double >, rgen_t > m_initial_orientation;
		fixed_or_random< double, boost::random::uniform_real_distribution< double >, rgen_t > m_initial_linear_dir;
		fixed_or_random< double, boost::random::uniform_real_distribution< double >, rgen_t > m_initial_linear_speed;
		fixed_or_random< double, boost::random::uniform_real_distribution< double >, rgen_t > m_initial_ang_vel;

	public:
		virtual agent_body* create_body(b2World* world) = 0;

		virtual agent_sensor_name_list get_sensors() const = 0;
		// TODO: Not sure how/where to deal with this. Part of i_agent/controller maybe?
		virtual size_t get_num_effectors() const = 0;
	};

}


#include <yaml-cpp/yaml.h>

namespace YAML {
	template <>
	struct convert < rtp::agent_body_spec::Type >
	{
		static Node encode(rtp::agent_body_spec::Type const& rhs)
		{
			return Node(rtp::agent_body_spec::Names[rhs]);
		}

		static bool decode(Node const& node, rtp::agent_body_spec::Type& rhs)
		{
			if(!node.IsScalar())
			{
				return false;
			}

			auto it = mapping_.find(node.Scalar());
			if(it == mapping_.end())
			{
				return false;
			}

			rhs = it->second;
			return true;
		}

		static std::map< std::string, rtp::agent_body_spec::Type > const mapping_;
	};
}


#endif

