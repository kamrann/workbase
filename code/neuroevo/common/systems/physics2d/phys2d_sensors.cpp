// phys2d_sensors.cpp

#include "phys2d_sensors.h"
#include "phys2d_entity_data.h"

#include "b2d_util.h"

#include <array>
#include <vector>


namespace sys {
	namespace phys2d {

		//void draw_indicator(Wt::WPainter& painter, b2Vec2 const& offset, bool state);

		struct directional_sensor: public phys_sensor
		{
			directional_sensor(phys_sensor_defn const& defn):
				m_body(defn.body),
				m_pos(defn.pos),
				m_angle(defn.orientation),
				m_range(defn.range)
			{
				m_dir = b2Mul(b2Rot(m_angle), b2Vec2(0.f, 1.f));
			}

			virtual sensor_value_t read() const override
			{
				auto world = m_body->GetWorld();

				RC_Callback cb;
				auto start = m_body->GetWorldPoint(m_pos);
				auto w_dir = m_body->GetWorldVector(m_dir);
				auto end = start + m_range * w_dir;
				world->RayCast(&cb, start, end);

				auto dist = cb.hit() ? (double)(cb.fraction * m_range) : std::numeric_limits< double >::max();
				return dist;
			}

/*			virtual void draw(Wt::WPainter& painter, b2Vec2 const& indicator_offset) const override
			{
				auto dist = read();
				bool object_sensed = dist != std::numeric_limits< double >::max();
				if(object_sensed)
				{
					painter.save();
					Wt::WPen pen(Wt::blue);
					painter.setPen(pen);
					painter.drawLine(m_pos.x, m_pos.y, m_pos.x + m_dir.x * dist, m_pos.y + m_dir.y * dist);
					painter.restore();
				}

				draw_indicator(painter, b2Mul(b2Rot(m_angle), indicator_offset), object_sensed);
			}
*/
			b2Body* m_body;
			b2Vec2 m_pos;
			float m_angle;
			b2Vec2 m_dir;
			float m_range;

			class RC_Callback: public b2RayCastCallback
			{
			public:
				RC_Callback(): fix(nullptr)
				{}

				float ReportFixture(b2Fixture* f, const b2Vec2& p, const b2Vec2& n, float x) override
				{
					fix = f;
					point = p;
					norm = n;
					fraction = x;
					return fraction;
				}

				inline bool hit() const
				{
					return fix != nullptr;
				}

				b2Fixture* fix;
				b2Vec2 point, norm;
				float fraction;
			};
		};


		struct fan_sensor: public contact_based_sensor
		{
			fan_sensor(phys_sensor_defn const& defn):
				m_angle(defn.orientation),
				m_count(0)
			{
				const int SegmentsPerQuadrant = 8;

				auto fan_data = boost::any_cast<fan_sensor_data>(defn.type_specific);

				const size_t num_segments = (int)std::ceil(SegmentsPerQuadrant * fan_data.field_of_view / (b2_pi / 2));
				const size_t max_segments_per_poly = b2_maxPolygonVertices - 2;
				const size_t num_polys = num_segments / max_segments_per_poly + (num_segments % max_segments_per_poly > 0 ? 1 : 0);
				size_t segment_idx = 0;
				for(size_t i = 0; i < num_polys; ++i)
				{
					std::vector< b2Vec2 > points;
					points.emplace_back(b2Vec2{ 0.f, 0.f });
					b2Vec2 base(0.f, defn.range);
					base = b2Mul(b2Rot(-fan_data.field_of_view / 2), base);

					points.emplace_back(b2Mul(b2Rot(fan_data.field_of_view * segment_idx / num_segments), base));
					auto end = std::min(segment_idx + max_segments_per_poly, num_segments);
					for(; segment_idx < end; ++segment_idx)
					{
						points.emplace_back(b2Mul(b2Rot(fan_data.field_of_view * (segment_idx + 1) / num_segments), base));
					}

					b2FixtureDef fd;
					b2PolygonShape poly;
					transform_points(points, b2Transform(defn.pos, b2Rot(defn.orientation)));
					poly.Set(points.data(), points.size());
					fd.shape = &poly;
					fd.isSensor = true;
					//fd.filter
					auto fix = defn.body->CreateFixture(&fd);
					set_fixture_data(fix, entity_fix_data{
						entity_fix_data::Type::Sensor,
						entity_fix_data::val_t{ static_cast<contact_based_sensor*>(this) }
					});
				}
			}

			virtual void on_contact(ContactType type) override
			{
				switch(type)
				{
					case ContactType::Begin:
					++m_count;
					break;
					case ContactType::End:
					--m_count;
					break;
				}
			}

			virtual sensor_value_t read() const override
			{
				// TODO: for debugging
				return m_count;
				//		return m_count > 0 ? 1.0 : 0.0;
			}

/*			virtual void draw(Wt::WPainter& painter, b2Vec2 const& indicator_offset) const override
			{
				draw_indicator(painter, b2Mul(b2Rot(m_angle), indicator_offset), read() > 0.0);
			}
*/
			float m_angle;
			size_t m_count;
		};


		std::unique_ptr< phys_sensor > create_sensor(phys_sensor_defn const& defn)
		{
			switch(defn.type)
			{
				case SensorType::Directional:
				return std::make_unique< directional_sensor >(defn);

				case SensorType::Fan:
				return std::make_unique< fan_sensor >(defn);

				default:
				throw std::exception("Invalid Sensor Type");
			}
		}

/*		void draw_indicator(Wt::WPainter& painter, b2Vec2 const& offset, bool state)
		{
			const float IndicatorSize = 0.4f;	// TODO: arg
			Wt::WRectF sensor_indicator_rc(-IndicatorSize / 2, -IndicatorSize / 2, IndicatorSize, IndicatorSize);
			painter.save();
			painter.setBrush(state ? Wt::WBrush(Wt::green) : Wt::BrushStyle(Wt::NoBrush));
			painter.translate(offset.x, offset.y);
			painter.drawEllipse(sensor_indicator_rc);
			painter.restore();
		}
*/
	}
}

