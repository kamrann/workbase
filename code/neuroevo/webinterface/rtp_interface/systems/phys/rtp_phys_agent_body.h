// rtp_phys_agent_body.h

#ifndef __NE_RTP_PHYS_AGENT_BODY_H
#define __NE_RTP_PHYS_AGENT_BODY_H

#include "../rtp_agent.h"
#include "rtp_phys_contacts.h"

#include "wt_param_widgets/pw_fwd.h"

//
namespace Wt {
	class WPainter;
}
//

//struct b2Vec2;
#include <Box2D/Box2D.h>

namespace rtp {

	class phys_system;
	struct phys_agent_specification;

	class agent_body: public i_agent
	{
	public:
		static std::string update_schema_providor(prm::schema::schema_provider_map_handle provider, prm::qualified_path const& prefix, bool evolvable);
//		static i_agent_factory* create_agent_factory(prm::param_accessor spec_acc, prm::param_accessor inst_acc);

	public:
		enum Sensors {
			Position_X,
			Position_Y,
			WorldVelocity_X,
			WorldVelocity_Y,
			KineticEnergy,

			_Next,
		};

		static agent_sensor_name_list sensor_inputs()
		{
			return{
				"Pos[x]",
				"Pos[y]",
				"Vel[x]",
				"Vel[y]",
				"KE",
			};
		}

	public:
//		agent_body() {}
		agent_body(phys_agent_specification const& spec, phys_system* system);

	public:
		virtual void translate(b2Vec2 const& vec) = 0;
		virtual void rotate(float angle) = 0;
		virtual void set_linear_velocity(b2Vec2 const& vel) = 0;
		virtual void set_angular_velocity(float vel) = 0;

		virtual b2Vec2 get_position() const = 0;
		virtual b2Vec2 get_linear_velocity() const = 0;
		virtual float get_kinetic_energy() const = 0;


		virtual agent_sensor_name_list get_sensors() const = 0;
		virtual agent_sensor_list get_mapped_inputs(std::vector< std::string > const& named_inputs) const;


		// TODO: Not sure how/where to deal with this. Part of i_agent/controller maybe?
		virtual size_t get_num_effectors() const = 0;

		virtual double get_sensor_value(agent_sensor_id const& sensor) const override;

		virtual void activate_effectors(std::vector< double > const& activations)
		{
			m_activations = activations;
		}

		virtual double get_effector_activation(size_t index)	// TODO: string id based??
		{
			return m_activations[index];
		}

		virtual void on_contact(b2Fixture* fixA, b2Fixture* fixB, ContactType type)
		{}

		virtual void on_collision(b2Fixture* fixA, b2Fixture* fixB, double approach_speed)
		{}

		// Temp, don't really want WPainter in the interface
		virtual void draw(Wt::WPainter& painter) const = 0;

		virtual std::string get_name() const
		{
			return m_name;
		}

	protected:
		// TODO: where is best ????
		std::vector< double > m_activations;

		std::string m_name;
	};

}


#endif

