// rtp_phys_entity_data.h

#ifndef __NE_RTP_PHYS_ENTITY_DATA_H
#define __NE_RTP_PHYS_ENTITY_DATA_H

#include <boost/any.hpp>


class b2Body;
class b2Fixture;
class b2Joint;

namespace rtp {

	struct entity_data
	{
		enum class Type {
			Undefined,

			Agent,
			Other,
		};

		typedef boost::any type_val_t;
		typedef boost::any val_t;

		Type type;
		type_val_t type_value;	// Specific to type (eg, for Agent, this will be an agent_body*)
		val_t value;			// Could be anything

		entity_data(): type(Type::Undefined)
		{}

		entity_data(Type type_, type_val_t&& type_value_): type(type_), type_value(type_value_)
		{}
	};

	struct entity_fix_data
	{
		enum class Type {
			Undefined,

			Sensor,
			Other,
		};

		typedef boost::any val_t;

		Type type;
		val_t value;

		entity_fix_data(): type(Type::Undefined)
		{}

		entity_fix_data(Type type_, val_t&& value_): type(type_), value(value_)
		{}
	};

	struct entity_joint_data
	{
/*		enum class Type {
			Undefined,

			Sensor,
			Other,
		};

		Type type;
*/
		typedef boost::any val_t;

		val_t value;

		entity_joint_data()//: type(Type::Undefined)
		{}
	};


	void set_body_data(b2Body* body, entity_data&& ed);
	void set_fixture_data(b2Fixture* fix, entity_fix_data&& efd);
	void set_joint_data(b2Joint* joint, entity_joint_data&& ejd);

	entity_data* get_body_data(b2Body* body);
	entity_fix_data* get_fixture_data(b2Fixture* fix);
	entity_joint_data* get_joint_data(b2Joint* joint);

}


#endif

