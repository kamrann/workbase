// basic_system_defn2.cpp

#include "basic_system_defn.h"
#include "agent_defn.h"
#include "controller_defn.h"

#include "system.h"
#include "agent.h"
#include "controller.h"
//#include "system_drawer.h"

#include "params/param.h"
#include "params/schema_builder.h"
#include "params/param_accessor.h"


namespace sys {

	void basic_system_defn::add_agent_defn(std::string role, agent_defn_ptr defn)
	{
		auto mp_it = m_agent_defns.insert(agent_defn_map_t::value_type{
			role,
			std::make_shared< agent_defn_list_t >()
		}).first;

		mp_it->second->push_back(std::move(defn));
	}

	void basic_system_defn::add_controller_defn(std::string controller_class, std::unique_ptr< i_controller_defn > defn)
	{
		auto mp_it = m_controller_defns.insert(controller_defn_map_t::value_type{
			controller_class,
			std::make_shared< controller_defn_list_t >()
		}).first;

		mp_it->second->push_back(std::move(defn));
	}

	namespace sb = prm::schema;

	std::string basic_system_defn::update_schema_provider(prm::schema::schema_provider_map_handle provider, prm::qualified_path const& prefix) const
	{
		auto path = prefix;

		auto core = std::string{ "core" };
		auto spec_list = std::string{ "spec_list" };
		auto inst_list = std::string{ "inst_list" };

		update_schema_provider_for_system_core(provider, path + core);
		update_schema_provider_for_agent_specification_list(provider, path + spec_list);
		update_schema_provider_for_agent_instance_list(provider, path + inst_list);

		(*provider)[path] = [=](prm::param_accessor acc)
		{
			auto s = sb::list(path.leaf().name());
			sb::append(s, provider->at(path + core)(acc));
			sb::append(s, provider->at(path + spec_list)(acc));
			sb::append(s, provider->at(path + inst_list)(acc));
			return s;
		};

		return path.leaf().name();
	}

	std::string basic_system_defn::update_schema_provider_for_agent_specification(prm::schema::schema_provider_map_handle provider, prm::qualified_path const& prefix) const
	{
		auto path = prefix;

		(*provider)[path + std::string("agent_role")] = [this](prm::param_accessor)
		{
			std::vector< std::string > roles;
			std::transform(
				std::begin(m_agent_defns),
				std::end(m_agent_defns),
				std::back_inserter(roles),
				[](agent_defn_map_t::value_type const& entry)
			{
				return entry.first;
			});
			auto s = sb::enum_selection(
				"agent_role",
				roles,
				0,
				1
				);
			sb::label(s, "Role");
			return s;
		};

		path += std::string("role_contents");

		(*provider)[path + std::string{ "agent_type" }] = [this](prm::param_accessor acc)
		{
			std::vector< std::string > agent_names;
			auto role_name = prm::extract_as< prm::enum_param_val >(acc["agent_role"])[0];
			auto const& agents = *m_agent_defns.at(role_name);
			for(auto const& a : agents)
			{
				agent_names.push_back(a->get_name());
			}

			auto s = sb::enum_selection(
				"agent_type",
				agent_names,
				0,
				1);
			return s;
		};

		path += std::string{ "agent_contents" };

		(*provider)[path + std::string("name")] = [](prm::param_accessor param_vals)
		{
			/*			auto default_name = std::string("Temp");
			auto path = param_vals.get_current_path();
			if(path.size() > 0)
			{
			auto node = param_vals["spec_type"];
			auto spec_type = node[0].as< Type >();
			default_name = Names[spec_type];

			default_name += "[";
			auto agent_it = path.find_anywhere("agent_list");
			default_name += std::to_string(agent_it->index());
			default_name += "]";

			default_name += "[";
			auto inst_it = path.find_anywhere("instance_list");
			default_name += std::to_string(inst_it->index());
			default_name += "]";
			}
			*/
			auto s = sb::string("name");// , default_name);
			sb::label(s, "Name");
			sb::trigger(s, "agentspec_name_modified");
			return s;
		};

		// Store the schema provider components for every registered agent type
		for(auto const& entry : m_agent_defns)
		{
			auto const& role_name = entry.first;
			auto const& defns = *entry.second;
			for(auto const& adefn : defns)
			{
				adefn->update_schema_providor_for_spec(provider, path + (adefn->get_name() + std::string{ "_spec" }));
			}
		}

		(*provider)[path] = [=](prm::param_accessor acc)
		{
			auto s = sb::list("agent_contents");
			if(acc.is_available("agent_type") && !prm::is_unspecified(acc["agent_type"]))
			{
				// A name for the spec
				sb::append(s, provider->at(path + std::string{ "name" })(acc));

				// Spec-type specific params
				auto agent_type_name = prm::extract_as< prm::enum_param_val >(acc["agent_type"])[0];
				sb::append(s, provider->at(path + (agent_type_name + std::string{ "_spec" }))(acc));
			}
			return s;
		};

		path.pop();

		(*provider)[path] = [=](prm::param_accessor acc)
		{
			auto s = sb::list("role_contents");
			if(acc.is_available("agent_role") && !prm::is_unspecified(acc["agent_role"]))
			{
				sb::append(s, provider->at(path + std::string{ "agent_type" })(acc));
				sb::append(s, provider->at(path + std::string{ "agent_contents" })(acc));
			}
			return s;
		};

		path.pop();

		(*provider)[path] = [=](prm::param_accessor acc)
		{
			auto s = sb::list(path.leaf().name());
			sb::append(s, provider->at(path + std::string("agent_role"))(acc));
			sb::append(s, provider->at(path + std::string("role_contents"))(acc));

			sb::border(s, std::string{ "Agent Specification" });
			return s;
		};

		return path.leaf().name();
	}

	// Returns a list of names of the defined agent specifications
	std::vector< std::pair< prm::qualified_path, std::string > > basic_system_defn::get_available_agent_specs(prm::param_accessor acc)
	{
		std::vector< std::pair< prm::qualified_path, std::string > > specs;

		acc.move_to(acc.find_path("spec_list"));
		auto spec_paths = acc.children();
		for(auto const& p : spec_paths)
		{
			acc.move_to(p);

			if(acc.is_available("name"))
			{
				auto name = prm::extract_as< std::string >(acc["name"]);
				if(!name.empty())
				{
					specs.push_back(std::make_pair(p, name));
				}
			}

			acc.revert();
		}

		return specs;
	}

	// Given a spec name, find the path to the spec param subtree
	prm::qualified_path basic_system_defn::get_agent_instance_spec_path(std::string spec_name, prm::param_accessor acc)
	{
		acc.move_to(acc.find_path("spec_list"));

		auto spec_paths = acc.children();
		for(auto const& p : spec_paths)
		{
			acc.move_to(p);

			auto name = prm::extract_as< std::string >(acc["name"]);
			if(name == spec_name)
			{
				return p;
			}

			acc.revert();
		}

		return{};
	}

	// Returns the name of the agent role for the agent spec at the current location of spec_acc
	std::string basic_system_defn::get_agent_role_name(prm::param_accessor spec_acc)
	{
		auto role_name_sel = prm::extract_as< prm::enum_param_val >(spec_acc["agent_role"]);
		return role_name_sel[0];
	}

	// Returns the name of the agent type for the agent spec at the current location of spec_acc
	std::string basic_system_defn::get_agent_type_name(prm::param_accessor spec_acc)
	{
		auto type_name_sel = prm::extract_as< prm::enum_param_val >(spec_acc["agent_type"]);
		return type_name_sel[0];
	}

	std::string basic_system_defn::update_schema_provider_for_agent_instance(prm::schema::schema_provider_map_handle provider, prm::qualified_path const& prefix) const
	{
		auto path = prefix;

		(*provider)[path + std::string{ "spec_reference" }] = [=](prm::param_accessor acc)
		{
			auto available_specs = get_available_agent_specs(acc);
			auto num_available_specs = available_specs.size();
			std::vector< std::pair< std::string, std::string > > spec_elements(num_available_specs);
			int idx = 0;
			std::transform(
				std::begin(available_specs),
				std::end(available_specs),
				begin(spec_elements),
				[](std::pair< prm::qualified_path, std::string > const& in)
			{
				return std::make_pair(
					in.second,// TODO: now using names to id, if keep this, need to enforce uniqueness
					// in.first.to_string(),
					in.second
					);
			});

			auto spec_ref = sb::enum_selection(
				"spec_reference",
				spec_elements,
				0, 1
				);
			sb::label(spec_ref, "Spec");
			sb::update_on(spec_ref, "$agentspec_name_modified");
			sb::update_on(spec_ref, "$agentspec_added_removed");

			return spec_ref;
		};

		// Store the instance schema provider components for every registered agent type
		for(auto const& entry : m_agent_defns)
		{
			auto const& role_name = entry.first;
			auto const& defns = *entry.second;
			for(auto const& adefn : defns)
			{
				adefn->update_schema_providor_for_instance(provider, path + (adefn->get_name() + std::string{ "_inst" }));
			}
		}

		(*provider)[path] = [=](prm::param_accessor acc)
		{
			auto s = sb::list(path.leaf().name());
			sb::append(s, provider->at(path + std::string("spec_reference"))(acc));
			if(acc.is_available("spec_reference") && !prm::is_unspecified(acc["spec_reference"]))
			{
				auto spec_name = prm::extract_as< prm::enum_param_val >(acc["spec_reference"])[0];
				auto agent_spec_path = get_agent_instance_spec_path(spec_name, acc);
				acc.move_to(agent_spec_path);
				auto agent_type_name = get_agent_type_name(acc);
				sb::append(s, provider->at(path + (agent_type_name + std::string{ "_inst" }))(acc));
			}
			sb::unborder(s);
			return s;
		};

		return path.leaf().name();
	}

	std::string basic_system_defn::update_schema_provider_for_agent_controller(prm::schema::schema_provider_map_handle provider, prm::qualified_path const& prefix) const
	{
		auto path = prefix;

		(*provider)[path + std::string("controller_class")] = [this](prm::param_accessor)
		{
			std::vector< std::string > classes;
			std::transform(
				std::begin(m_controller_defns),
				std::end(m_controller_defns),
				std::back_inserter(classes),
				[](controller_defn_map_t::value_type const& entry)
			{
				return entry.first;
			});
			auto s = sb::enum_selection(
				"controller_class",
				classes,
				0,
				1
				);
			sb::label(s, "Class");
			return s;
		};

		path += std::string("cclass_contents");

		(*provider)[path + std::string{ "controller_type" }] = [this](prm::param_accessor acc)
		{
			std::vector< std::string > controller_names;
			auto class_name = prm::extract_as< prm::enum_param_val >(acc["controller_class"])[0];
			auto const& controllers = *m_controller_defns.at(class_name);
			for(auto const& c : controllers)
			{
				controller_names.push_back(c->get_name());
			}

			auto s = sb::enum_selection(
				"controller_type",
				controller_names,
				0,
				1);
			return s;
		};
		
		path += std::string{ "ctype_contents" };

		// Store the schema provider components for every registered controller type
		for(auto const& entry : m_controller_defns)
		{
			auto const& class_name = entry.first;
			auto const& defns = *entry.second;
			for(auto const& cdefn : defns)
			{
				cdefn->update_schema_providor(provider, path + cdefn->get_name());
			}
		}

		(*provider)[path] = [=](prm::param_accessor acc)
		{
			auto s = sb::list("ctype_contents");
			if(acc.is_available("controller_type") && !prm::is_unspecified(acc["controller_type"]))
			{
				auto controller_name = prm::extract_as< prm::enum_param_val >(acc["controller_type"])[0];
				sb::append(s, provider->at(path + controller_name)(acc));
			}
			return s;
		};

		path.pop();

		(*provider)[path] = [=](prm::param_accessor acc)
		{
			auto s = sb::list("cclass_contents");
			if(acc.is_available("controller_class") && !prm::is_unspecified(acc["controller_class"]))
			{
				sb::append(s, provider->at(path + std::string{ "controller_type" })(acc));
				sb::append(s, provider->at(path + std::string{ "ctype_contents" })(acc));
			}
			return s;
		};

		path.pop();

		(*provider)[path] = [=](prm::param_accessor acc)
		{
			auto s = sb::list(path.leaf().name());
			sb::append(s, provider->at(path + std::string("controller_class"))(acc));
			sb::append(s, provider->at(path + std::string("cclass_contents"))(acc));

			sb::border(s, std::string{ "Controller" });
			return s;
		};

		return path.leaf().name();
	}

	std::string basic_system_defn::update_schema_provider_for_agent_specification_list(prm::schema::schema_provider_map_handle provider, prm::qualified_path const& prefix) const
	{
		auto path = prefix;

		auto spec = std::string{ "agent_spec" };
		update_schema_provider_for_agent_specification(provider, path + spec);

		(*provider)[path] = [=](prm::param_accessor acc)
		{
			auto s = sb::repeating_list(path.leaf().name(), spec, 0, 0);
			sb::label(s, "Agent Specificatons");
			sb::trigger(s, "agentspec_added_removed");
			//sb::enable_import(s, "physics2d.agent_spec_list");
			return s;
		};

		return path.leaf().name();
	}

	std::string basic_system_defn::update_schema_provider_for_agent_instance_list(prm::schema::schema_provider_map_handle provider, prm::qualified_path const& prefix) const
	{
		auto path = prefix;

		path += std::string{ "agent_inst" };

		auto inst = std::string{ "inst_contents" };
		update_schema_provider_for_agent_instance(provider, path + inst);
		auto con = std::string{ "inst_controller" };
		update_schema_provider_for_agent_controller(provider, path + con);

		(*provider)[path] = [=](prm::param_accessor acc)
		{
			auto s = sb::list(path.leaf().name());
			sb::append(s, provider->at(path + inst)(acc));
			sb::append(s, provider->at(path + con)(acc));
			return s;
		};

		path.pop();

		(*provider)[path] = [=](prm::param_accessor acc)
		{
			auto s = sb::repeating_list(path.leaf().name(), "agent_inst", 0, 0);
			// TODO:			sb::trigger(s, "agent_modified");
			sb::label(s, "Agent Instances");
			//sb::enable_import(s, "physics2d.agent_instance_list");
			return s;
		};

		return path.leaf().name();
	}


	state_value_id_list basic_system_defn::get_system_state_values(prm::param_accessor acc) const
	{
		// Perhaps classify values in hierarchical string form.
		// eg. asteroids/<index>/speed, agents/role/agent_name/agent_value, etc.
		// then in create_system, lock in the bindings

		auto svs = get_system_core_state_values(acc);

		auto inst_list_path = acc.find_path("inst_list");
		if(inst_list_path)
		{
			acc.move_to(inst_list_path);
			// For every agent instance...
			auto inst_paths = acc.children();
			for(auto const& p : inst_paths)
			{
				acc.move_to(p);
				auto spec_name = get_agent_instance_spec_name(acc);
				if(!spec_name)
				{
					// As yet unspecified
					continue;
				}
				auto agent_spec_path = get_agent_instance_spec_path(*spec_name, acc);

				acc.move_to(agent_spec_path);
				auto agent_role_name = get_agent_role_name(acc);
				auto agent_type_name = get_agent_type_name(acc);
				auto agent_defn = get_agent_defn(agent_role_name, agent_type_name);

				auto agent_svs = agent_defn->get_agent_state_values(acc);
				svs.insert(
					std::end(svs),
					std::begin(agent_svs),
					std::end(agent_svs)
					);
				acc.revert();
				acc.revert();
			}
		}
		return svs;
	}

	std::vector< std::string > basic_system_defn::get_agent_type_names() const
	{
		// TODO: Provide roll as argument?
		std::vector< std::string > names;
		for(auto const& role : m_agent_defns)
		{
			auto const& agents = *role.second;
			for(auto const& a : agents)
			{
				names.push_back(a->get_name());
			}
		}
		return names;
	}

	std::vector< std::string > basic_system_defn::get_agent_sensor_names(prm::param agent_type, prm::param_accessor param_vals) const
	{
		// TODO: through virtual of i_agent_defn
		return{};
	}

	size_t basic_system_defn::get_agent_num_effectors(prm::param_accessor spec_acc) const
	{
		auto role = get_agent_role_name(spec_acc);
		auto type = get_agent_type_name(spec_acc);
		auto defn = get_agent_defn(role, type);
		return defn->num_effectors(spec_acc);
	}

	std::vector< prm::qualified_path > basic_system_defn::get_connected_agent_specs(std::string controller_cls, std::string controller_type, prm::param_accessor acc) const
	{
		std::vector< prm::qualified_path > result;
		auto inst_list_path = acc.find_path("inst_list");
		if(inst_list_path)
		{
			acc.move_to(inst_list_path);
			// For every agent instance...
			auto inst_paths = acc.children();
			for(auto const& p : inst_paths)
			{
				auto acc2 = acc;
				acc2.move_to(p);

				acc2.move_relative(std::string{ "inst_controller" });

				auto cls_name = prm::extract_as< prm::enum_param_val >(acc2["controller_class"])[0];
				if(cls_name != controller_cls)
				{
					continue;
				}

				auto type_name = prm::extract_as< prm::enum_param_val >(acc2["controller_type"])[0];
				if(type_name != controller_type)
				{
					continue;
				}

				acc2.revert();

				auto spec_name = get_agent_instance_spec_name(acc2);
				if(!spec_name)
				{
					// As yet unspecified
					continue;
				}
				auto agent_spec_path = get_agent_instance_spec_path(*spec_name, acc2);
				acc2.revert();

				result.push_back(agent_spec_path);
			}
		}
		return result;
	}

	boost::optional< std::string > basic_system_defn::get_agent_instance_spec_name(prm::param_accessor acc)
	{
		auto spec_sel = prm::extract_as< prm::enum_param_val >(acc["spec_reference"]);
		return prm::is_unspecified(acc["spec_reference"]) ? boost::none : boost::make_optional(spec_sel[0]);
	}

	i_agent_defn* basic_system_defn::get_agent_defn(std::string const& role, std::string const& type) const
	{
		auto& role_defns = *m_agent_defns.at(role);
		auto a_it = std::find_if(
			std::begin(role_defns),
			std::end(role_defns),
			[type](agent_defn_ptr const& a)
		{
			return a->get_name() == type;
		});

		if(a_it == std::end(role_defns))
		{
			return nullptr;
		}
		else
		{
			return a_it->get();
		}
	}

	i_controller_defn* basic_system_defn::get_controller_defn(std::string const& cls, std::string const& type) const
	{
		auto& cls_defns = *m_controller_defns.at(cls);
		auto c_it = std::find_if(
			std::begin(cls_defns),
			std::end(cls_defns),
			[type](controller_defn_ptr const& con)
		{
			return con->get_name() == type;
		});

		if(c_it == std::end(cls_defns))
		{
			return nullptr;
		}
		else
		{
			return c_it->get();
		}
	}

	system_ptr basic_system_defn::create_system(prm::param_accessor acc) const
	{
		//auto controller = create_controller(acc);
		//sys->register_agent_controller(0, std::move(controller));

		auto sys = create_system_core(acc);

		// Now create and register agents and their controllers
		acc.move_to(acc.find_path("inst_list"));
		// For every agent instance...
		auto inst_paths = acc.children();
		for(auto const& p : inst_paths)
		{
			acc.move_to(p);

			auto spec_name = get_agent_instance_spec_name(acc);
			auto agent_spec_path = get_agent_instance_spec_path(*spec_name, acc);

			auto spec_acc = acc;
			spec_acc.move_to(agent_spec_path);
			auto agent_role_name = get_agent_role_name(spec_acc);
			auto agent_type_name = get_agent_type_name(spec_acc);
			auto agent_defn = get_agent_defn(agent_role_name, agent_type_name);

			auto agent = agent_defn->create_agent(spec_acc, acc);
			auto agent_id = sys->register_agent(std::move(agent));
			// TODO: Maybe not necessary. Could leave to system implementation to do within register_agent()
			//static_cast<basic_agent*>(agent.get())->create(sys.get());

			auto con_cls = prm::extract_as< prm::enum_param_val >(acc["controller_class"])[0];
			auto con_type = prm::extract_as< prm::enum_param_val >(acc["controller_type"])[0];

			auto con_defn = get_controller_defn(con_cls, con_type);
			auto controller = con_defn->create_controller(acc);

			sys->register_agent_controller(agent_id, std::move(controller));

			acc.revert();
		}

		return std::move(sys);
	}

}



