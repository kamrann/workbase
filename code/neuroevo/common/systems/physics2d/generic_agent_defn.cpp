// generic_agent_defn.cpp

#include "generic_agent_defn.h"
#include "generic_agent.h"
#include "component_object.h"

#include "b2d_script/parsing/load_script.h"


namespace sys {
	namespace phys2d {

		std::string generic_agent_defn::get_name() const
		{
			return "generic_agent";
		}

		ddl::defn_node generic_agent_defn::get_spec_defn(ddl::specifier& spc)
		{
			ddl::defn_node script_file = spc.string("script")
				;

			auto rf_script = ddl::node_ref{ script_file };
			get_script_object_fn_.set_fn([this, rf_script](ddl::navigator nav) -> std::shared_ptr< component_object >
			{
				nav = ddl::nav_to_ref(rf_script, nav);
				if(nav)
				{
					auto script_file = nav.get().as_string();
					auto script = b2ds::load_script_from_file(script_file);
					if(script)
					{
						auto world = std::make_unique< b2World >(b2Vec2(0.0f, 0.0f));
						return component_object::create_from_script(*script, world.get());
					}
				}

				return nullptr;
			});
			get_script_object_fn_.add_dependency(ddl::node_dependency(rf_script));

			agent_state_values_fn_.set_fn([this](ddl::navigator nav) -> state_value_id_list
			{
				auto obj = get_script_object_fn_(nav);
				if(obj)
				{
					return obj->get_state_values();
				}
				else
				{
					return{};
				}
			});
			agent_state_values_fn_.add_dependency(get_script_object_fn_);

			num_effectors_fn_.set_fn([this](ddl::navigator nav) -> size_t
			{
				auto obj = get_script_object_fn_(nav);
				if(obj)
				{
					return obj->num_activations();
				}
				else
				{
					return 0;
				}
			});
			num_effectors_fn_.add_dependency(get_script_object_fn_);

			return spc.composite("generic_agent")(ddl::define_children{}
				("script", script_file)
				);
		}

		ddl::dep_function< state_value_id_list > generic_agent_defn::get_agent_state_values_fn() const
		{
			return agent_state_values_fn_;
		}

		state_value_id_list generic_agent_defn::get_agent_state_values(ddl::navigator nav) const
		{
			return agent_state_values_fn_(nav);
		}

		std::vector< std::string > generic_agent_defn::sensor_inputs(ddl::navigator nav) const
		{
			return{};
		}

		ddl::dep_function< size_t > generic_agent_defn::num_effectors_fn() const
		{
			return num_effectors_fn_;
		}

		size_t generic_agent_defn::num_effectors(ddl::navigator nav) const
		{
			return num_effectors_fn_(nav);
		}

		agent_ptr generic_agent_defn::create_agent(ddl::navigator spec_nav, ddl::navigator inst_nav) const
		{
			auto spec = std::make_shared< spec_data >();

			spec->script = spec_nav["script"].get().as_string();

			auto inst = std::make_shared< instance_data >();
			phys2d_agent_defn::initialize_instance_data(*inst, inst_nav);

			return std::make_unique< generic_agent >(spec, inst);
		}

	}
}


