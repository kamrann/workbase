// load_script.cpp

#include "load_script.h"

#include "object_parser.h"
#include "b2d_components/components/rotation_joint.h"
#include "b2d_components/components/sucker_cmp.h"

#include <boost/spirit/include/qi.hpp>

#include <map>
#include <fstream>


namespace b2ds {

	boost::optional< script > load_script_from_file(std::string const& script_filepath)
	{
		std::ifstream file(script_filepath);
		if(!file.is_open())
		{
			//			std::cout << "Failed to open file" << std::endl;
			return boost::none;
		}

		auto text = std::string{
			std::istreambuf_iterator<char>(file),
			std::istreambuf_iterator<char>()
		};

		return script{ text };
	}

	struct component_visitor: public boost::static_visitor < bool >
	{
		inline result_type operator() (ast::body& c)
		{
			bodies.push_back(std::move(c));
			if(bodies.back().name)
			{
				named_bodies[*bodies.back().name] = &bodies.back();
			}
			return true;
		}

		inline result_type operator() (ast::joint& c)
		{
			if(!c.props.anchors || !c.props.max_torque || !c.props.control)
			{
				return false;
			}

			auto const& b1anchor = c.props.anchors->b1;
			auto const& b2anchor = c.props.anchors->b2;
			if(named_bodies.find(b1anchor.ref) == std::end(named_bodies))
			{
				return false;
			}
			if(named_bodies.find(b2anchor.ref) == std::end(named_bodies))
			{
				return false;
			}
			if(b1anchor.ref == b2anchor.ref)
			{
				return false;
			}

			joints.push_back(std::move(c));
			return true;
		}

		inline result_type operator() (ast::sucker& c)
		{
			if(named_bodies.find(c.location.ref) == std::end(named_bodies))
			{
				return false;
			}

			suckers.push_back(std::move(c));
			return true;
		}
		
		inline result_type operator() (ast::obj_rotation& c)
		{
			if(!rot)
			{
				rot = std::move(c);
				return true;
			}
			return false;
		}

		inline result_type operator() (ast::obj_base& c)
		{
			if(!base)
			{
				base = std::move(c);
				return true;
			}
			return false;
		}

		std::list< ast::body > bodies;
		std::list< ast::joint > joints;
		std::list< ast::sucker > suckers;
		boost::optional< ast::obj_rotation > rot;
		boost::optional< ast::obj_base > base;

		std::map< std::string, ast::body* > named_bodies;
	};

	struct create_rotation_joint_visitor:
		public boost::static_visitor < std::shared_ptr< b2dc::rotation_joint > >
	{
		create_rotation_joint_visitor(
			b2Body* b1,
			b2Vec2 b1anchor,
			b2Body* b2,
			b2Vec2 b2anchor,
			boost::optional< ast::joint_limits > limits,
			ast::max_torque max_torque
			):
			b1_(b1),
			b2_(b2),
			b1anchor_(b1anchor),
			b2anchor_(b2anchor),
			limits_(limits),
			max_torque_(max_torque)
		{}

		result_type operator() (ast::torque_control const& c) const
		{
			return b2dc::rotation_joint::create_torque_activation(
				b1_,
				b1anchor_,
				b2_,
				b2anchor_,
				max_torque_,
				limits_
				);
		}

		result_type operator() (ast::speed_control const& c) const
		{
			return b2dc::rotation_joint::create_speed_activation(
				b1_,
				b1anchor_,
				b2_,
				b2anchor_,
				max_torque_,
				*c.max_speed,
				limits_
				);
		}

		result_type operator() (ast::positional_control const& c) const
		{
			return b2dc::rotation_joint::create_position_activation(
				b1_,
				b1anchor_,
				b2_,
				b2anchor_,
				max_torque_,
				*c.gain,
				c.max_speed,
				*limits_
				);
		}

		b2Body *b1_, *b2_;
		b2Vec2 b1anchor_, b2anchor_;
		boost::optional< ast::joint_limits > limits_;
		ast::max_torque max_torque_;
	};

	std::shared_ptr< b2dc::rotation_joint > create_rotation_joint(
		b2Body* b1,
		b2Vec2 b1anchor,
		b2Body* b2,
		b2Vec2 b2anchor,
		boost::optional< ast::joint_limits > limits,
		ast::max_torque max_torque,
		ast::control control
		)
	{
		auto vis = create_rotation_joint_visitor{
			b1, b1anchor,
			b2, b2anchor,
			limits,
			max_torque
		};
		return control.apply_visitor(vis);
	}


	bool instantiate_script(script const& s, b2World* world, instantiation_data* id)
	{
		object_parser< std::string::const_iterator > p;

		auto it = std::begin(s);
		ast::object obj;
		auto success = qi::phrase_parse(it, std::end(s), p, qi::space_type{}, obj);
		success = success && it == std::end(s);

		if(!success)
		{
			return false;
		}

		component_visitor extracted;
		for(auto& comp : obj.components)
		{
			if(!boost::apply_visitor(extracted, comp))
			{
				return false;
			}
		}

		if(extracted.bodies.empty())
		{
			return false;
		}

		if(!extracted.base)
		{
			return false;
		}

		unsigned int unnamed_body_count = 0;
		unsigned int unnamed_hinge_count = 0;
		unsigned int unnamed_sucker_count = 0;

		std::map< std::string, b2Body* > named_bodies;
		for(auto const& b : extracted.bodies)
		{
			b2BodyDef bd;
			bd.type = b2_dynamicBody;
			bd.position.Set(b.pos.x, b.pos.y);
			auto body = world->CreateBody(&bd);

			b2PolygonShape shape;
			shape.SetAsBox(b.shape.width / 2.0, b.shape.height / 2.0,
				b2Vec2(b.shape.width / 2.0, b.shape.height / 2.0), 0.0);
			b2FixtureDef fd;
			// todo: default material
			fd.density = 1.0;
			fd.friction = 0.5;
			fd.restitution = 0.0;
			if(b.mat)
			{
				// TODO: material preset props for b.mat->type
				// ...
				if(b.mat->props)
				{
					if(b.mat->props->density)
					{
						fd.density = *b.mat->props->density;
					}
					if(b.mat->props->friction)
					{
						fd.friction = *b.mat->props->friction;
					}
					if(b.mat->props->restitution)
					{
						fd.restitution = *b.mat->props->restitution;
					}
				}
			}
			fd.shape = &shape;
			auto fix = body->CreateFixture(&fd);

			auto name = b.name ? *b.name :
				(std::string{ "unnamed_body_" } + std::to_string(++unnamed_body_count));
			named_bodies[name] = body;
		}

		std::map< std::string, std::shared_ptr< b2dc::rotation_joint > > hinges;
		for(auto const& j : extracted.joints)
		{
			auto const& b1anchor = j.props.anchors->b1;
			auto const& b2anchor = j.props.anchors->b2;
			
			auto joint = create_rotation_joint(
				named_bodies.at(b1anchor.ref),
				b2Vec2(
				0.5 + b1anchor.crd.x,
				0.5 + b1anchor.crd.y
				),
				named_bodies.at(b2anchor.ref),
				b2Vec2(
				0.5 + b2anchor.crd.x,
				0.5 + b2anchor.crd.y
				),
				j.props.limits,
				*j.props.max_torque,
				*j.props.control
				);

			auto name = j.name ? *j.name :
				(std::string{ "unnamed_hinge_" } +std::to_string(++unnamed_hinge_count));
			hinges[name] = joint;
		}

		std::map< std::string, std::shared_ptr< b2dc::sucker_cmp > > suckers;
		for(auto const& s : extracted.suckers)
		{
			auto sucker = b2dc::sucker_cmp::create(
				named_bodies.at(s.location.ref),
				b2Vec2(0.5 + s.location.crd.x, 0.5 + s.location.crd.y),
				1.0,	// todo:
				b2dc::collision_filter{}	// todo:
				);

			auto name = s.name ? *s.name :
				(std::string{ "unnamed_sucker_" } +std::to_string(++unnamed_sucker_count));
			suckers[name] = sucker;
		}

		if(id)
		{
			id->bodies = std::move(named_bodies);
			id->hinges = std::move(hinges);
			id->suckers = std::move(suckers);
		}

		return true;
	}

}




