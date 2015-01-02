// rotation_joint.cpp

#include "rotation_joint.h"


namespace b2dc {

	std::shared_ptr< rotation_joint > rotation_joint::create_torque_activation(
		b2Body* b1, b2Vec2 b1anchor,
		b2Body* b2, b2Vec2 b2anchor,
		value_t max_torque,
		boost::optional< limits_t > limits,
		bool collide
		)
	{
		b2RevoluteJointDef jd;
		if(limits)
		{
			jd.enableLimit = true;
			jd.lowerAngle = limits->lower;
			jd.upperAngle = limits->upper;
		}
		jd.collideConnected = collide;

		jd.bodyA = b1;
		jd.localAnchorA = b1anchor;
		jd.bodyB = b2;
		jd.localAnchorB = b2anchor;

		generic_activation_state g_as;
		g_as.max_theoretical_torque = max_torque;
		torque_activation_state as;
		return std::make_shared< rotation_joint >(g_as, as, (b2RevoluteJoint*)b1->GetWorld()->CreateJoint(&jd));//jd);
	}

	std::shared_ptr< rotation_joint > rotation_joint::create_speed_activation(
		b2Body* b1, b2Vec2 b1anchor,
		b2Body* b2, b2Vec2 b2anchor,
		value_t max_torque,
		value_t max_speed,
		boost::optional< limits_t > limits,
		bool collide
		)
	{
		b2RevoluteJointDef jd;
		if(limits)
		{
			jd.enableLimit = true;
			jd.lowerAngle = limits->lower;
			jd.upperAngle = limits->upper;
		}
		jd.enableMotor = true;
		jd.collideConnected = collide;

		jd.bodyA = b1;
		jd.localAnchorA = b1anchor;
		jd.bodyB = b2;
		jd.localAnchorB = b2anchor;

		generic_activation_state g_as;
		g_as.max_theoretical_torque = max_torque;
		speed_activation_state as;
		as.max_speed = max_speed;
		return std::make_shared< rotation_joint >(g_as, as, (b2RevoluteJoint*)b1->GetWorld()->CreateJoint(&jd));//jd);
	}

	std::shared_ptr< rotation_joint > rotation_joint::create_position_activation(
		b2Body* b1, b2Vec2 b1anchor,
		b2Body* b2, b2Vec2 b2anchor,
		value_t max_torque,
		value_t gain,
		boost::optional< value_t > max_speed,
		limits_t limits,
		bool collide
		)
	{
		b2RevoluteJointDef jd;
		jd.enableLimit = true;
		jd.lowerAngle = limits.lower;
		jd.upperAngle = limits.upper;
		jd.enableMotor = true;
		jd.collideConnected = collide;

		jd.bodyA = b1;
		jd.localAnchorA = b1anchor;
		jd.bodyB = b2;
		jd.localAnchorB = b2anchor;

		generic_activation_state g_as;
		g_as.max_theoretical_torque = max_torque;
		position_activation_state as;
		as.gain = gain;
		// TODO: optional max speed
		return std::make_shared< rotation_joint >(g_as, as, (b2RevoluteJoint*)b1->GetWorld()->CreateJoint(&jd));//jd);
	}

	rotation_joint::rotation_joint(generic_activation_state const& g_as, torque_activation_state const& tas, b2RevoluteJoint* jnt): //b2RevoluteJointDef const& jd):
		activation_type_{ Activation::Torque }
		, gen_as_{ g_as }
		, type_as_{ tas }
	{
		joint_ = jnt;// world->CreateJoint(&jd);
	}

	rotation_joint::rotation_joint(generic_activation_state const& g_as, speed_activation_state const& sas, b2RevoluteJoint* jnt): //b2RevoluteJointDef const& jd):
		activation_type_{ Activation::Speed }
		, gen_as_{ g_as }
		, type_as_{ sas }
	{
		joint_ = jnt;// world->CreateJoint(&jd);
	}

	rotation_joint::rotation_joint(generic_activation_state const& g_as, position_activation_state const& pas, b2RevoluteJoint* jnt): //b2RevoluteJointDef const& jd):
		activation_type_{ Activation::Position }
		, gen_as_{ g_as }
		, type_as_{ pas }
	{
		joint_ = jnt;// world->CreateJoint(&jd);
	}

	void rotation_joint::tq_apply_activation(value_t act)
	{
		/*
		Needs to be configurable, but for now, assume that the activation value is a modulation of
		a max_theoretical_torque value. If muscle energy is being simulated, this will be clamped
		to within a max_applicable_torque value determined by the state of the muscle.

		This means the controller is requesting a theoretical response, so a value of 1.0 means it
		ideally wants the max_theoretical_torque to be applied, regardless of current muscle state
		and whether or not this is possible.

		An alternative would be for the controller to request a specified amount of 'effort', so requesting
		responses in a relative fashion. In this case there would be no subsequent clamping, just a
		modulation of the max_applicable_torque.
		*/

		auto const& as = get_act_st< Activation::Torque >();

		auto act_magnitude = std::abs(act);
		auto max_applicable = gen_as_.max_theoretical_torque;	// todo: clamp based on muscle energy
		auto requested = act_magnitude * gen_as_.max_theoretical_torque;
		auto torque = std::copysign(std::min(requested, max_applicable), act);

		apply_torque(torque);
		gen_as_.applied_torque = torque;
	}

	void rotation_joint::spd_apply_activation(value_t act)
	{
		/*
		Activation is a modulation of a max_speed value.
		// TODO: Should allow for different speed caps in +ve and -ve rotation directions.

		A motor is used which attempts to maintain the desired speed. This motor is subject to the
		max_theoretical_torque, and also to optional muscle energy.
		*/

		auto const& as = get_act_st< Activation::Speed >();

		joint_->SetMotorSpeed(act * as.max_speed);
		joint_->SetMaxMotorTorque(gen_as_.max_theoretical_torque);	// todo: muscle energy, as above

		gen_as_.applied_torque = joint_->GetMotorTorque(30.0);	// TODO: !!!!!!!!!!!!
	}

	void rotation_joint::pos_apply_activation(value_t act)
	{
		/*
		Activation specifies an absolute angle by a mapping from [0, 1] to [min, max], where min and
		max are the joint limits. Note that currently this activation method requires the joint
		to have limits enabled.

		A motor is used to attempt to track to the angle, under the same conditions as above.
		*/

		auto const& as = get_act_st< Activation::Position >();

		auto target = joint_->GetLowerLimit() + (joint_->GetUpperLimit() - joint_->GetLowerLimit()) * act;
		auto error = get_angle() - target;
		joint_->SetMotorSpeed(-as.gain * error);
		joint_->SetMaxMotorTorque(gen_as_.max_theoretical_torque);	// todo: muscle energy, as above

		gen_as_.applied_torque = joint_->GetMotorTorque(30.0);	// TODO: !!!!!!!!!!!!
	}

}





