// phys2d_agent_defn.cpp

#include "phys2d_agent_defn.h"
#include "phys2d_object.h"

#include "params/param_accessor.h"
#include "params/schema_builder.h"


namespace sb = prm::schema;

namespace sys {
	namespace phys2d {

		std::string phys2d_agent_defn::update_schema_providor_for_instance(prm::schema::schema_provider_map_handle provider, prm::qualified_path const& prefix) const
		{
			auto path = prefix;

			// TODO: following dependent on agent type
			// this class will probably become useless and just override in relevant xxx_agent_defn

			path += std::string{ "initial_conditions" };

			(*provider)[path + std::string("init_x")] = [](prm::param_accessor)
			{
				auto s = sb::random("init_x", 0.0, boost::none, boost::none, -10.0, 10.0);
				sb::label(s, "X Pos");
				return s;
			};
			(*provider)[path + std::string("init_y")] = [](prm::param_accessor)
			{
				auto s = sb::random("init_y", 0.0, boost::none, boost::none, -10.0, 10.0);
				sb::label(s, "Y Pos");
				return s;
			};
			(*provider)[path + std::string("init_orientation")] = [](prm::param_accessor)
			{
				auto s = sb::random("init_orientation", 0.0, -180.0, 180.0);
				sb::label(s, "Orientation");
				return s;
			};
			(*provider)[path + std::string("init_linear_angle")] = [](prm::param_accessor)
			{
				auto s = sb::random("init_linear_angle", 0.0, 0.0, 360.0);
				sb::label(s, "Linear Motion Angle");
				return s;
			};
			(*provider)[path + std::string("init_linear_speed")] = [](prm::param_accessor)
			{
				auto s = sb::random("init_linear_speed", 0.0, 0.0, boost::none, 0.0, 10.0);
				sb::label(s, "Linear Speed");
				return s;
			};
			(*provider)[path + std::string("init_ang_vel")] = [](prm::param_accessor)
			{
				auto s = sb::random("init_ang_vel", 0.0, boost::none, boost::none, -90.0, 90.0);
				sb::label(s, "Angular Velocity");
				return s;
			};

			(*provider)[path] = [=](prm::param_accessor acc)
			{
				auto s = sb::list("initial_conditions");
				sb::append(s, provider->at(path + std::string("init_x"))(acc));
				sb::append(s, provider->at(path + std::string("init_y"))(acc));
				sb::append(s, provider->at(path + std::string("init_orientation"))(acc));
				sb::append(s, provider->at(path + std::string("init_linear_angle"))(acc));
				sb::append(s, provider->at(path + std::string("init_linear_speed"))(acc));
				sb::append(s, provider->at(path + std::string("init_ang_vel"))(acc));
				sb::label(s, "Initial Conditions");
				return s;
			};

			path.pop();

			(*provider)[path] = [=](prm::param_accessor acc)
			{
				auto s = sb::list(path.leaf().name());
				sb::append(s, provider->at(path + std::string("initial_conditions"))(acc));
				sb::unborder(s);
				//			sb::enable_import(s, "TODO:");
				return s;
			};

			return path.leaf().name();
		}

		void phys2d_agent_defn::initialize_instance_data(instance_data& inst, prm::param_accessor acc)
		{
			inst.pos_x = prm::extract_fixed_or_random(acc["init_x"]);
			inst.pos_y = prm::extract_fixed_or_random(acc["init_y"]);
			inst.orientation = prm::extract_fixed_or_random(acc["init_orientation"]) * (b2_pi / 180);
			inst.linvel_angle = prm::extract_fixed_or_random(acc["init_linear_angle"]) * (b2_pi / 180);
			inst.linvel_mag = prm::extract_fixed_or_random(acc["init_linear_speed"]);
			inst.angvel = prm::extract_fixed_or_random(acc["init_ang_vel"]) * (b2_pi / 180);
		}

		void phys2d_agent_defn::initialize_object_state(object* obj, instance_data const& st, rgen_t& rgen)
		{
			auto init_orientation = st.orientation.get(rgen);
			obj->rotate(init_orientation);

			auto init_x = st.pos_x.get(rgen);
			auto init_y = st.pos_y.get(rgen);
			obj->translate(b2Vec2(init_x, init_y));

			auto motion_angle = st.linvel_angle.get(rgen);
			auto linear_speed = st.linvel_mag.get(rgen);
			auto linvel = DimensionalityTraits< WorldDimensionality::dim2D >::bearing_to_vector(motion_angle) * linear_speed;
			obj->set_linear_velocity(b2Vec2(linvel[0], linvel[1]));

			auto angular_vel = st.angvel.get(rgen);
			obj->set_angular_velocity(angular_vel);
		}

	}
}


