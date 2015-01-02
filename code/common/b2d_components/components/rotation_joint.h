// rotation_joint.h

#ifndef __NE_PHYS2D_ROTATION_JOINT_H
#define __NE_PHYS2D_ROTATION_JOINT_H

#include <box2d/box2d.h>

#include <boost/optional.hpp>
#include <boost/variant.hpp>


namespace b2dc {

	class rotation_joint
	{
	public:
		typedef double value_t;
		
		struct limits_t
		{
			value_t lower;
			value_t upper;

			limits_t():
				lower(0.0),
				upper(0.0)
			{}
			limits_t(value_t l, value_t u):
				lower(l),
				upper(u)
			{}
		};

		enum class Activation {
			None,			// Passive joint
			Torque,			// Activated directly by torque
			Speed,			// Activated with desired speed
			Position,		// Activated with desired angle to track to
		};

		struct generic_activation_state
		{
			value_t max_theoretical_torque;
			// todo: optional muscle energy settings

			value_t applied_torque;

			generic_activation_state():
				max_theoretical_torque{ 0.0 },
				applied_torque{ 0.0 }
			{}
		};

		struct torque_activation_state//:
			//				public generic_activation_state
		{

		};

		struct speed_activation_state//:
			//				public generic_activation_state
		{
			value_t max_speed;
		};

		struct position_activation_state//:
			//				public generic_activation_state
		{
			value_t gain;	// higher value -> faster tracking to the requested angle
		};

		typedef boost::variant <
			torque_activation_state,
			speed_activation_state,
			position_activation_state
		> type_activation_state_t;

	public:
		static std::shared_ptr< rotation_joint > create_torque_activation(
			b2Body* b1, b2Vec2 b1anchor, 
			b2Body* b2, b2Vec2 b2anchor,
			value_t max_torque,
			boost::optional< limits_t > limits = boost::none,
			bool collide = false
			);

		static std::shared_ptr< rotation_joint > create_speed_activation(
			b2Body* b1, b2Vec2 b1anchor,
			b2Body* b2, b2Vec2 b2anchor,
			value_t max_torque,
			value_t max_speed,
			boost::optional< limits_t > limits = boost::none,
			bool collide = false
			);

		static std::shared_ptr< rotation_joint > create_position_activation(
			b2Body* b1, b2Vec2 b1anchor,
			b2Body* b2, b2Vec2 b2anchor,
			value_t max_torque,
			value_t gain,
			boost::optional< value_t > max_speed,
			limits_t limits,
			bool collide = false
			);

	public:
		inline value_t get_angle() const
		{
			return joint_->GetJointAngle();
		}

		inline value_t get_speed() const
		{
			return joint_->GetJointSpeed();
		}

		inline value_t get_applied_torque() const
		{
			return gen_as_.applied_torque;
		}

		inline void apply_activation(value_t act)
		{
			switch(activation_type_)
			{
				case Activation::Torque:
				tq_apply_activation(act);
				break;
				case Activation::Speed:
				spd_apply_activation(act);
				break;
				case Activation::Position:
				pos_apply_activation(act);
				break;
			}
		}

	public:	// should be private but can't work with make_shared
		rotation_joint(generic_activation_state const& g_as, torque_activation_state const& tas, b2RevoluteJoint* jnt);//b2RevoluteJointDef const& jd);
		rotation_joint(generic_activation_state const& g_as, speed_activation_state const& sas, b2RevoluteJoint* jnt);//b2RevoluteJointDef const& jd);
		rotation_joint(generic_activation_state const& g_as, position_activation_state const& pas, b2RevoluteJoint* jnt);//b2RevoluteJointDef const& jd);

	protected:
		template < Activation act >
		struct as_from_type;

		template <>
		struct as_from_type < Activation::Torque >
		{
			typedef torque_activation_state type;
		};

		template <>
		struct as_from_type < Activation::Speed >
		{
			typedef speed_activation_state type;
		};

		template <>
		struct as_from_type < Activation::Position >
		{
			typedef position_activation_state type;
		};

		template < Activation act >
		inline auto get_act_st() const -> typename as_from_type< act >::type const&
		{
			return boost::get< typename as_from_type< act >::type /*const&*/ >(type_as_);
		}

		template < Activation act >
		inline auto get_act_st() -> typename as_from_type< act >::type&
		{
			return boost::get< typename as_from_type< act >::type /*&*/ >(type_as_);
		}

		inline void apply_torque(value_t tq)
		{
			joint_->GetBodyA()->ApplyTorque(-tq, true);
			joint_->GetBodyB()->ApplyTorque(tq, true);
		}

	protected:
		void tq_apply_activation(value_t act);
		void spd_apply_activation(value_t act);
		void pos_apply_activation(value_t act);

	protected:
		b2RevoluteJoint* joint_;
		Activation activation_type_;
		generic_activation_state gen_as_;
		type_activation_state_t type_as_;
	};

}


#endif

