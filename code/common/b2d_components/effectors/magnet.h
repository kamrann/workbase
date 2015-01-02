// magnet.h

#ifndef __NE_PHYS2D_MAGNET_H
#define __NE_PHYS2D_MAGNET_H

#include <box2d/box2d.h>


namespace b2dc {

	class magnet
	{
	public:
		static std::shared_ptr< magnet > create(
			b2Body* body
			, b2Vec2 local_point
			, double strength		// 1.0 = force of 1N on metallic mass of 1kg at radius 1m
			// todo: dropoff range - specify at what field strength should treat as zero
			);

	public:
		void switch_on();
		void switch_off();
		inline bool is_activated() const
		{
			return active_;
		}

		void update();

	public:	// should be private but can't work with make_shared
		magnet(b2Body* body, b2Vec2 local_point, double strength);

	protected:
		b2Body* body_;
		b2Fixture* sensor_;
		b2Vec2 local_point_;
		double strength_;
		bool active_;
		//			std::vector< b2Body* > in_range_;
	};

}



#endif

