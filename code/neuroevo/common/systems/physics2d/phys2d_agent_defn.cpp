// phys2d_agent_defn.cpp

#include "phys2d_agent_defn.h"
#include "phys2d_object.h"

#include "util/dimensionality.h"


namespace sys {
	namespace phys2d {

		ddl::defn_node phys2d_agent_defn::get_instance_defn(ddl::specifier& spc)
		{
			// TODO: following dependent on agent type
			// this class will probably become useless and just override in relevant xxx_agent_defn

			ddl::defn_node init_conditions = spc.composite("initial_conditions")(ddl::define_children{}
				("init_x", spc.realnum("init_x"))
				("init_y", spc.realnum("init_y"))
				("init_orientation", spc.realnum("init_orientation")(ddl::spc_range< ddl::realnum_defn_node::value_t >{ -180., 180. }))
				("init_linear_angle", spc.realnum("init_linear_angle")(ddl::spc_range< ddl::realnum_defn_node::value_t >{ 0., 360. }))
				("init_linear_speed", spc.realnum("init_linear_speed")(ddl::spc_range < ddl::realnum_defn_node::value_t > { 0., boost::none }))
				("init_angvel", spc.realnum("init_angvel"))
				);

			return spc.composite("phys2d_agent_inst")(ddl::define_children{}
				("initial_conditions", init_conditions)
				);
		}

		void phys2d_agent_defn::initialize_instance_data(instance_data& inst, ddl::navigator nav)
		{
			nav = nav["initial_conditions"];
			inst.pos_x = nav["init_x"].get().as_real();
			inst.pos_y = nav["init_y"].get().as_real();
			inst.orientation = nav["init_orientation"].get().as_real() * (b2_pi / 180);
			inst.linvel_angle = nav["init_linear_angle"].get().as_real() * (b2_pi / 180);
			inst.linvel_mag = nav["init_linear_speed"].get().as_real();
			inst.angvel = nav["init_angvel"].get().as_real() * (b2_pi / 180);
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

			// NOTE: Using auto here fails. Seems like a bug in Eigen, maybe fixed in latest version?
			Eigen::Vector2d linvel = DimensionalityTraits< WorldDimensionality::dim2D >::bearing_to_vector(motion_angle) * linear_speed;
			obj->set_linear_velocity(b2Vec2(linvel[0], linvel[1]));

			auto angular_vel = st.angvel.get(rgen);
			obj->set_angular_velocity(angular_vel);
		}

	}
}


