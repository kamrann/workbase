// b2d_util.h

#ifndef __BOX2D_UTIL_H
#define __BOX2D_UTIL_H

#include <Box2D/Box2D.h>


inline float get_kinetic_energy(b2Body const* b)
{
	float const m = b->GetMass();
	float const v2 = b->GetLinearVelocity().LengthSquared();
	float const I = b->GetInertia();
	float const w = b->GetAngularVelocity();
	return 0.5f * (m * v2 + I * w * w);
}

inline float get_grav_potential_energy(b2Body const* b, float const g, float const ground = 0.0f)
{
	return b->GetMass() * g * (b->GetPosition().y - ground);
}

template < typename Container, typename XForm >
inline void transform_points(Container& points, XForm const& xf)
{
	for(auto& v : points)
	{
		v = b2Mul(xf, v);
	}
}


namespace Wt {
	class WPainter;
}

void draw_body(b2Body const* b, Wt::WPainter& painter);



#endif


