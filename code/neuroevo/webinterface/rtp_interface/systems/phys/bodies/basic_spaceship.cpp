// basic_spaceship.cpp

#include "basic_spaceship.h"
#include "../../../params/paramlist_par.h"
#include "wt_param_widgets/pw_yaml.h"

#include "thrusters/thruster_presets.h"

#include <Wt/WPainter>

#include "Box2D/Box2D.h"


namespace rtp_phys {

	rtp_param_type* basic_spaceship::spec::params()
	{
		rtp_named_param_list p = agent_body_spec::base_params();
		// Add more here
		return new rtp_staticparamlist_param_type(p);
	}

	YAML::Node basic_spaceship::spec::get_schema(YAML::Node const& param_vals)
	{
		prm::schema_builder sb;

		sb.add_integer("Placeholder", 0);

		return sb.get_schema();
	}

	basic_spaceship::spec* basic_spaceship::spec::create_instance(rtp_param param)
	{
		spec* s = new spec();
		agent_body_spec::create_base_instance(param, s);
		return s;
	}

	agent_body* basic_spaceship::spec::create_body(b2World* world)
	{
		return new basic_spaceship(*this, world);
	}

	basic_spaceship::spec::spec(): agent_body_spec(agent_body_spec::Type::Spaceship)
	{}

	basic_spaceship::basic_spaceship(spec const& spc, b2World* world):
		m_t_cfg(new thruster_config< WorldDimensionality::dim2D >(thruster_presets::square_minimal())),
		m_t_system(m_t_cfg)
	{
		m_half_width = 2.0f;

		b2BodyDef bd;
		bd.type = b2_dynamicBody;
		bd.position.Set(0.0f, 0.0f);
		bd.angle = 0.0f;
		m_body = world->CreateBody(&bd);

		b2PolygonShape shape;
		shape.SetAsBox(m_half_width, m_half_width);

		b2FixtureDef fd;
		fd.shape = &shape;
		fd.density = 1.0f;
		m_body->CreateFixture(&fd);
	}

	void basic_spaceship::draw(Wt::WPainter& painter) const
	{
		simple_rigid_body::draw(painter);

/*		Wt::WRectF rc(
			-m_half_width,
			-m_half_width,
			m_half_width * 2,
			m_half_width * 2
			);
*/		painter.save();
		painter.translate(m_body->GetPosition().x, m_body->GetPosition().y);
		painter.rotate(m_body->GetAngle() * 360.0 / (2 * b2_pi));
//		painter.drawRect(rc);

		thruster_config< WorldDimensionality::dim2D > const& t_cfg = *m_t_cfg;
		double const ThrusterSize = m_half_width * 0.25;
		std::array< Wt::WPointF, 3 > points = {
			Wt::WPointF(0, 0),
			Wt::WPointF(-ThrusterSize / 2, -ThrusterSize),
			Wt::WPointF(ThrusterSize / 2, -ThrusterSize)
		};
		Wt::WPen pen(Wt::GlobalColor::black);
		for(size_t i = 0; i < m_t_cfg->num_thrusters(); ++i)
		{
			painter.save();

			auto const& tpos = m_half_width * t_cfg[i].pos;
			painter.translate(tpos[0], tpos[1]);
			double dir_x = t_cfg[i].dir[0];
			double dir_y = t_cfg[i].dir[1];
			double theta = boost::math::copysign(std::acos(dir_y), dir_x);
			painter.rotate(360.0 * -theta / (2 * boost::math::double_constants::pi));

			double temperature = m_t_system[i].t.temperature();
			Wt::WColor color((int)(temperature * 255), 0, 0);
			pen.setColor(color);
			painter.setPen(pen);
			Wt::WBrush br(color);
			painter.setBrush(br);
			painter.drawPolygon(&points[0], points.size());

			painter.restore();
		}

		painter.restore();
	}
}



