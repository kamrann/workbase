// generic_agent.cpp

#include "generic_agent.h"
#include "generic_agent_defn.h"
#include "phys2d_system.h"
#include "phys2d_entity_data.h"

#include "b2d_script/parsing/load_script.h"


namespace sys {
	namespace phys2d {

		std::string generic_agent::get_name() const
		{
			return "generic_agent";	// Instance name?
		}

		size_t generic_agent::initialize_state_value_bindings(sv_bindings_t& bindings, sv_accessors_t& accessors) const
		{
			//return component_object::initialize_state_value_bindings(bindings, accessors);
			return cmp_obj_->initialize_state_value_bindings(bindings, accessors);
		}

		agent_sensor_list generic_agent::get_mapped_inputs(std::vector< std::string > const& named_inputs) const
		{
			return{};
		}

		double generic_agent::get_sensor_value(agent_sensor_id const& sensor) const
		{
			return{};
		}

		generic_agent::generic_agent(
			std::shared_ptr< generic_agent_defn::spec_data > _spec,
			std::shared_ptr< generic_agent_defn::instance_data > _inst)
		{
			m_spec = _spec;
			m_inst = _inst;
		}

		void generic_agent::create(i_system* sys)
		{
			auto phys2d_sys = static_cast<phys2d_system /*const*/ *>(sys);
//			m_sys = phys2d_sys;
			auto world = phys2d_sys->get_world();

			auto script = b2ds::load_script_from_file(m_spec->script);
			cmp_obj_ = component_object::create_from_script(*script, world);
			//

			// Initial conditions
// todo:			phys2d_agent_defn::initialize_object_state(this, *m_inst, sys->get_rgen());
		}

		void generic_agent::activate_effectors(effector_activations const& activations)
		{
			//component_object::activate_effectors(activations);
			cmp_obj_->activate_effectors(activations);
		}

	}
}



