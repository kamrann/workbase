// rtp_phys_entity_data.cpp

#include "rtp_phys_entity_data.h"

#include <Box2D/Box2D.h>


namespace rtp {

	void set_body_data(b2Body* body, entity_data&& ed)
	{
		body->SetUserData(new entity_data(std::move(ed)));
	}

	void set_fixture_data(b2Fixture* fix, entity_fix_data&& efd)
	{
		fix->SetUserData(new entity_fix_data(std::move(efd)));
	}

	void set_joint_data(b2Joint* joint, entity_joint_data&& ejd)
	{
		joint->SetUserData(new entity_joint_data(std::move(ejd)));
	}

	entity_data* get_body_data(b2Body* body)
	{
		auto ud = body->GetUserData();
		return ud ? static_cast<entity_data*>(ud) : nullptr;
	}

	entity_fix_data* get_fixture_data(b2Fixture* fix)
	{
		auto ud = fix->GetUserData();
		return ud ? static_cast<entity_fix_data*>(ud) : nullptr;
	}

	entity_joint_data* get_joint_data(b2Joint* joint)
	{
		auto ud = joint->GetUserData();
		return ud ? static_cast<entity_joint_data*>(ud) : nullptr;
	}

}



