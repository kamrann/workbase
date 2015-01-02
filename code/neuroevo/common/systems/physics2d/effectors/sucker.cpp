// sucker.cpp

#include "sucker.h"


namespace sys {
	namespace phys2d {

		std::shared_ptr< sucker > sucker::create(
			b2Body* body
			, b2Vec2 local_point
//			, double max_force
			)
		{
			return std::make_shared< sucker >(body, local_point);
		}

		sucker::sucker(b2Body* body, b2Vec2 local_point):
			body_{ body }
			, local_point_{ local_point }
			, joint_{ nullptr }
		{
			
		}

		void sucker::attach(b2Body* target)
		{
			b2WeldJointDef jd;
			jd.collideConnected = true;
			jd.frequencyHz = 30.0;	// TODO: ?
			jd.dampingRatio = 1.0;	// ?

			jd.Initialize(body_, target,
				body_->GetWorldPoint(local_point_));

			joint_ = (b2WeldJoint*)body_->GetWorld()->CreateJoint(&jd);
		}

		void sucker::detach()
		{
			body_->GetWorld()->DestroyJoint(joint_);
			joint_ = nullptr;
		}

	}
}




