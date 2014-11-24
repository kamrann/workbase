// b2d_util.cpp

#include "b2d_util.h"

#include <Wt/WPainter>

#include <Box2D/Box2D.h>


void draw_body(b2Body const* b, Wt::WPainter& painter)
{
	painter.save();
	painter.translate(b->GetPosition().x, b->GetPosition().y);
	painter.rotate(b->GetAngle() * 360.0 / (2 * b2_pi));

	b2Fixture const* fix = b->GetFixtureList();
	while(fix)
	{
		bool is_sensor = fix->IsSensor();
		Wt::WPen pen(is_sensor ? Wt::PenStyle::DotLine : Wt::PenStyle::SolidLine);
		pen.setColor(is_sensor ? Wt::blue : Wt::black);
		painter.setPen(pen);
		painter.setBrush(Wt::WBrush(Wt::NoBrush));

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

			case b2Shape::e_edge:
			{
				auto edge = (b2EdgeShape const*)shape;
				painter.drawLine(
					edge->m_vertex1.x,
					edge->m_vertex1.y,
					edge->m_vertex2.x,
					edge->m_vertex2.y
					);
			}
			break;

			// TODO: chain
		}

		fix = fix->GetNext();
	}

	painter.restore();
}


