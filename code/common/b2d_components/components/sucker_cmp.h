// sucker_cmp.h

#ifndef __B2D_COMPONENTS_SUCKER_CMP_H
#define __B2D_COMPONENTS_SUCKER_CMP_H

#include "../effectors/sucker.h"
#include "../sensors/basic_contact_sensor.h"


namespace b2dc {

	class sucker_cmp
	{
	public:
		static std::shared_ptr< sucker_cmp > create(
			b2Body* body
			, b2Vec2 local_point
			, double range
			//				, double max_force
			, collision_filter sensor_filter
			);

	public:
		inline bool is_attached() const
		{
			return sucker_->is_attached();
		}

		inline bool can_attach() const
		{
			return sensor_->is_contacting();
		}

		inline bool attach()
		{
			if(!is_attached() && can_attach())
			{
				sucker_->attach(sensor_->get_contact_body());
				return true;
			}
			else
			{
				return false;
			}
		}

		inline bool detach()
		{
			if(is_attached())
			{
				sucker_->detach();
				return true;
			}
			else
			{
				return false;
			}
		}

	public:	// should be private but can't work with make_shared
		sucker_cmp(std::shared_ptr< sucker > sck, std::shared_ptr< basic_contact_sensor > sen);

	protected:
		std::shared_ptr< sucker > sucker_;
		std::shared_ptr< basic_contact_sensor > sensor_;
	};

}



#endif

