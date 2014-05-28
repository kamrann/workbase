// b2d_util.h

#ifndef __BOX2D_UTIL_H
#define __BOX2D_UTIL_H

#include <Wt/WPainter>

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

inline void draw_body(b2Body const* b, Wt::WPainter& painter)
{
	painter.save();
	painter.translate(b->GetPosition().x, b->GetPosition().y);
	painter.rotate(b->GetAngle() * 360.0 / (2 * b2_pi));

	b2Fixture const* fix = b->GetFixtureList();
	while(fix)
	{
		b2Shape const* shape = fix->GetShape();
		switch(shape->GetType())
		{
			case b2Shape::e_polygon:
			{
				b2PolygonShape const* polygon = (b2PolygonShape const*)shape;
				size_t const count = polygon->GetVertexCount();
				std::vector< Wt::WPointF > points(count);
				for(size_t i = 0; i < count; ++i)
				{
					b2Vec2 const& v = polygon->GetVertex(i);
					points[i] = Wt::WPointF(v.x, v.y);
				}
				painter.drawPolygon(&points[0], points.size());
			}
			break;

			case b2Shape::e_circle:
			{
				b2CircleShape const* circle = (b2CircleShape const*)shape;
				Wt::WRectF rc(
					circle->m_p.x - circle->m_radius,
					circle->m_p.y - circle->m_radius,
					circle->m_radius * 2,
					circle->m_radius * 2
					);
				painter.drawEllipse(rc);
			}
			break;

			// TODO: edge/chain
		}

		fix = fix->GetNext();
	}

	painter.restore();
}


#endif


