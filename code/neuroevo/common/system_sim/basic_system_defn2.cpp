// basic_system_defn2.cpp

#include "basic_system_defn.h"
#include "agent_defn.h"
#include "controller_defn.h"

#include "system.h"
#include "agent.h"
#include "controller.h"


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

	ddl::defn_node basic_system_defn::get_defn(ddl::specifier& spc)
	{
		using ddl::defn_node;

		defn_node core = get_system_core_defn(spc);
		auto spec_list = get_agent_specification_list_defn(spc);
		auto inst_list = get_agent_instance_list_defn(spc);

		//////////////////

		auto rf_core = ddl::node_ref{ core };	// TODO: if we have fully specified dependencies/triggers, shouldn't need this, since it is only used to position a navigator currently
		auto rf_any_inst_spec_ref = ddl::node_ref{ inst_spec_ref_dn_, ddl::node_ref::Resolution::Any };
		get_system_state_values_fn_.set_fn([this, rf_core, rf_any_inst_spec_ref](ddl::navigator nav) -> state_value_id_list
		{
			// Perhaps classify values in hierarchical string form.
			// eg. asteroids/<index>/speed, agents/role/agent_name/agent_value, etc.
			// then in create_system, lock in the bindings

			// TODO: Dependencies of this call...
			auto svs = get_system_core_state_values_fn()(nav);
				//ddl::nav_to_ref(rf_core, nav));

			// For every agent instance
			auto resolved = ddl::resolve_reference(rf_any_inst_spec_ref, nav);
			for(auto ref : resolved)
			{
				auto rnav = ddl::navigator{ nav.tree_, ref.nd };
				auto isr_values = rnav.get().as_enum();
				if(isr_values.empty())
				{
					// No spec yet defined for this instance, skip.
					continue;
				}

				auto spec_nav = get_inst_spec_nav_fn_(rnav);
				auto role_type = get_spec_role_fn_(spec_nav);
				auto spec_type = get_spec_type_fn_(spec_nav);

				auto agent_defn = get_agent_defn(*role_type, *spec_type);

				auto agent_svs = agent_defn->get_agent_state_values_fn()(spec_nav);
				svs.insert(
					std::end(svs),
					std::begin(agent_svs),
					std::end(agent_svs)
					);
			}
			return svs;
		});
		get_system_state_values_fn_.add_dependency(ddl::node_dependency(rf_any_inst_spec_ref));
		get_system_state_values_fn_.add_dependency(get_system_core_state_values_fn());
		for(auto const& role_entry : m_agent_defns)
		{
			auto const& role_defns = *role_entry.second;
			for(auto const& adefn : role_defns)
			{
				get_system_state_values_fn_.add_dependency(adefn->get_agent_state_values_fn());
			}
		}
		get_system_state_values_fn_.add_dependency(get_inst_spec_nav_fn_);
		get_system_state_values_fn_.add_dependency(get_spec_role_fn_);
		get_system_state_values_fn_.add_dependency(get_spec_type_fn_);
		//////////////////

		auto rf_inst_list = ddl::node_ref{ inst_list };
		get_connected_spec_navs_fn_impl_ = [this, rf_inst_list](
			std::string controller_cls,
			std::string controller_type,
			ddl::navigator nav) -> std::vector< ddl::navigator >
		{
			std::vector< ddl::navigator > result;
			nav = ddl::nav_to_ref(rf_inst_list, nav);
			auto inst_count = nav.list_num_items();
			// For every agent instance...
			for(size_t i = 0; i < inst_count; ++i)
			{
				auto inst_nav = nav[i];

				auto cclass = get_controller_class_fn_(inst_nav);
				if(!cclass || *cclass != controller_cls)
				{
					continue;
				}

				auto ctype = get_controller_type_fn_(inst_nav);
				if(!ctype || ctype != controller_type)
				{
					continue;
				}

				auto spec_nav = get_inst_spec_nav_fn_(inst_nav);
				if(!spec_nav)
				{
					// As yet unspecified
					continue;
				}
				
				result.push_back(spec_nav);
			}
			return result;
		};
		get_connected_spec_navs_fn_.add_dependency(ddl::node_dependency(rf_inst_list));
		get_connected_spec_navs_fn_.add_dependency(get_controller_class_fn_);
		get_connected_spec_navs_fn_.add_dependency(get_controller_type_fn_);
		get_connected_spec_navs_fn_.add_dependency(get_inst_spec_nav_fn_);

		return spc.composite("basic_system")(ddl::define_children{}
			("core", core)
			("specs", spec_list)
			("instances", inst_list)
			);
	}

	ddl::defn_node basic_system_defn::get_agent_specification_defn(ddl::specifier& spc)
	{
		using ddl::defn_node;

		// TODO: Probably can omit this and used fixed enum, since we assume that all agent defns are registered
		// before use.
		auto get_agent_roles_fn = ddl::enum_defn_node::enum_values_str_fn_t{ [this](ddl::navigator nav)
		{
			ddl::enum_defn_node::enum_str_set_t roles;
			std::transform(
				std::begin(m_agent_defns),
				std::end(m_agent_defns),
				std::inserter(roles, std::end(roles)),
				[](agent_defn_map_t::value_type const& entry)
			{
				return entry.first;
			});
			return roles;
		}
		};

		defn_node role_type = spc.enumeration("role_type")
			(ddl::spc_range< size_t >{ 1, 1 })
			(ddl::define_enum_func{ get_agent_roles_fn })
			;
		////////
		auto rf_role = ddl::node_ref{ role_type };
		get_spec_role_fn_.set_fn([rf_role](ddl::navigator nav) -> boost::optional< std::string >
		{
			nav = ddl::nav_to_ref(rf_role, nav);
			if(nav)
			{
				auto vnode = nav.get();
				if(!vnode.as_enum().empty())
				{
					return vnode.as_single_enum_str();
				}
			}

			return boost::none;
		});
		get_spec_role_fn_.add_dependency(ddl::node_dependency(rf_role));
		////////

		auto get_agent_types_fn = ddl::enum_defn_node::enum_values_str_fn_t{ [this, rf_role](ddl::navigator nav)
		{
			nav = ddl::nav_to_ref(rf_role, nav);
			ddl::enum_defn_node::enum_str_set_t agent_names;
			auto role_name = nav.get().as_single_enum_str();
			auto const& agents = *m_agent_defns.at(role_name);
			for(auto const& a : agents)
			{
				agent_names.insert(std::end(agent_names), a->get_name());
			}
			return agent_names;
		}
		};
		// TODO: Need a nice way to ensure that any node reference captured and used in the lambda function
		// is also registered as a dependency
		get_agent_types_fn.add_dependency(ddl::node_dependency(rf_role));

		defn_node agent_type = spc.enumeration("agent_type")
			(ddl::spc_range< size_t >{ 1, 1 })
			(ddl::define_enum_func{ get_agent_types_fn })
			;
		////////
		auto rf_type = ddl::node_ref{ agent_type };
		get_spec_type_fn_.set_fn([rf_type](ddl::navigator nav) -> boost::optional< std::string >
		{
			nav = ddl::nav_to_ref(rf_type, nav);
			if(nav)
			{
				auto vnode = nav.get();
				if(!vnode.as_enum().empty())
				{
					return vnode.as_single_enum_str();
				}
			}

			return boost::none;
		});
		get_spec_type_fn_.add_dependency(ddl::node_dependency(rf_type));
		////////

		defn_node spec_name = spc.string("spec_name");
		spec_name_dn_ = spec_name;
		///////////
		auto rf_spec_name = ddl::node_ref{ spec_name, ddl::node_ref::Resolution::Any };
		get_all_spec_names_fn_.set_fn([rf_spec_name](ddl::navigator nav)
		{
			std::vector< std::string > specs;

			auto resolved = ddl::resolve_reference(rf_spec_name, nav);
			for(auto ref : resolved)
			{
				auto rnav = ddl::navigator{ nav.tree_, ref.nd };
				auto name = rnav.get().as_string();
				if(!name.empty())
				{
					specs.push_back(name);
				}
			}

			return specs;
		});
		get_all_spec_names_fn_.add_dependency(ddl::node_dependency(rf_spec_name));
		///////////

		auto agent_details_spc = spc.conditional("agent_details_?");
		for(auto const& entry : m_agent_defns)
		{
			auto const& role_name = entry.first;
			auto const& defns = *entry.second;
			for(auto const& adefn : defns)
			{
				defn_node dn = adefn->get_spec_defn(spc);
				agent_details_spc = agent_details_spc
					// TODO: just checking agent type name, should really check role also!?
					(ddl::spc_condition{ ddl::cnd::equal{ ddl::node_ref{ agent_type }, adefn->get_name() }, dn });
			}
		}

		defn_node agent_details = agent_details_spc;

		defn_node agent_contents = spc.composite("agent_contents")(ddl::define_children{}
			("name", spec_name)
			("details", agent_details)
			);

		defn_node role_contents = spc.composite("role_contents")(ddl::define_children{}
			("agent_type", agent_type)
			("agent_contents", spc.conditional("agent_contents_?")(ddl::spc_condition{ ddl::cnd::is_selection{ ddl::node_ref{ agent_type } }, agent_contents }))
			);

		return spc.composite("agent_spec")(ddl::define_children{}
			("agent_role", role_type)
			("role_contents", spc.conditional("role_contents_?")(ddl::spc_condition{ ddl::cnd::is_selection{ ddl::node_ref{ role_type } }, role_contents }))
			);
	}
#if 0
	// Returns a list of names of the defined agent specifications
	std::vector< std::pair< int /* TODO: node_ref? */, std::string > > basic_system_defn::get_available_agent_specs(ddl::navigator nav) const
	{
		std::vector< std::pair< int, std::string > > specs;

		nav = ddl::nav_to_ref(ddl::node_ref{ dn_spec_list_ }, nav);
		auto count = nav.list_num_items();
		for(size_t idx = 0; idx < count; ++idx)
		{
			auto cnav = nav[idx];
			if(cnav.is_child("name"))	// TODO: possible to do this using node_ref instead of hard coded string lookup??
			{
				auto name = cnav["name"].get().as_string();
				if(!name.empty())
				{
					specs.push_back(std::make_pair(int{}, name));
				}
			}
		}

		return specs;
	}

	// Given a spec name, find the path to the spec param subtree
	ddl::navigator basic_system_defn::get_agent_instance_spec_nav(std::string spec_name, ddl::navigator nav) const
	{
		nav = ddl::nav_to_ref(ddl::node_ref{ dn_spec_list_ }, nav);
		auto spec_count = nav.list_num_items();
		for(size_t idx = 0; idx < spec_count; ++idx)
		{
			auto cnav = nav[idx];
			auto name = cnav["name"].get().as_string();
			if(name == spec_name)
			{
				return cnav;
			}
		}

		return{};
	}

	// Returns the name of the agent role for the agent spec at the current location of spec_acc
	std::string basic_system_defn::get_agent_role_name(ddl::navigator spec_nav) const
	{
		return spec_nav["agent_role"].get().as_single_enum_str();
	}

	// Returns the name of the agent type for the agent spec at the current location of spec_acc
	std::string basic_system_defn::get_agent_type_name(ddl::navigator spec_nav) const
	{
		return spec_nav["agent_type"].get().as_single_enum_str();
	}
#endif
	ddl::defn_node basic_system_defn::get_agent_instance_defn(ddl::specifier& spc)
	{
		using ddl::defn_node;

		auto get_agent_spec_names_fn = ddl::enum_defn_node::enum_values_str_fn_t{ [this](ddl::navigator nav)
		{
			ddl::enum_defn_node::enum_str_set_t names;	// todo: store a node_ref or something as the enum data component?
			auto specs = get_all_spec_names_fn_(nav);
			std::transform(
				std::begin(specs),
				std::end(specs),
				std::inserter(names, std::end(names)),
				[](std::string const& entry)
			{
				return entry;
			});
			return names;
		}
		};
		// TODO: This dependency comes from the call to get_available_agent_specs() in the above lambda.
		// Perhaps that function should itself be a dep_function, so that we don't have to keep track of indirect
		// dependencies in the code.
		// FURTHER: This particular dependency is more complex, as it really needs to be a dependency not only
		// on the spec list node, but on a number of subnodes within the spec defn (and indeed on every instance
		// that it created within the list). Maybe allowing to create arbitrary named dependencies (like the
		// events in the old param tree) which a node defn can be set to trigger would help here?
		get_agent_spec_names_fn.add_dependency(get_all_spec_names_fn_);

		ddl::defn_node spec_ref = spc.enumeration("agent_inst_spec_ref")
			(ddl::spc_range< size_t >{ 1, 1 })
			(ddl::define_enum_func{ get_agent_spec_names_fn })
			;
		inst_spec_ref_dn_ = spec_ref;

		auto rf_spec_ref = ddl::node_ref{ spec_ref };
		get_inst_spec_ref_fn_.set_fn([rf_spec_ref](ddl::navigator nav) -> boost::optional< std::string >
		{
			nav = ddl::nav_to_ref(rf_spec_ref, nav);
			if(nav)
			{
				auto node = nav.get();
				if(!node.as_enum().empty())
				{
					return node.as_single_enum_str();
				}
			}

			return boost::none;
		});
		get_inst_spec_ref_fn_.add_dependency(ddl::node_dependency(rf_spec_ref));


		//////////////////
		/* TODO: this is done here since we have dependency on a defn created as part of the spec defn, and
		// also on the above get_inst_spec_ref_fn_. yet it must be defined before the below custom
		// conditions since they make use of it, and dependency functions are copied by value.
		// rather than store refs as basic_system_defn
		// class members, may be better to have objects of spec_defn and inst_defn (or a component which wraps them)
		// stored, and query that component for the desired internal defn which is chooses to expose.
		*/
		auto get_inst_spec_ref_fn = get_inst_spec_ref_fn_;
		auto rf_any_spec_name = ddl::node_ref{ spec_name_dn_, ddl::node_ref::Resolution::Any };
		get_inst_spec_nav_fn_.set_fn([get_inst_spec_ref_fn, rf_any_spec_name](ddl::navigator nav) -> ddl::navigator
		{
			// Get the spec reference name from the current instance
			auto ref_name = get_inst_spec_ref_fn(nav);
			if(!ref_name)
			{
				return{};
			}

			// Now resolve in turn every spec name
			auto resolved = ddl::resolve_reference(rf_any_spec_name, nav);
			for(auto ref : resolved)
			{
				auto rnav = ddl::navigator{ nav.tree_, ref.nd };
				auto name = rnav.get().as_string();
				if(name == ref_name)
				{
					// Successful match
					return rnav.parent();
				}
			}

			return{};
		});
		get_inst_spec_nav_fn_.add_dependency(get_inst_spec_ref_fn);
		get_inst_spec_nav_fn_.add_dependency(ddl::node_dependency(rf_any_spec_name));
		/////////////////////////

		get_inst_num_effectors_fn_.set_fn([this](ddl::navigator nav) -> size_t
		{
			auto spec_nav = get_inst_spec_nav_fn_(nav);
			if(!spec_nav)
			{
				return 0;
			}

			auto role = get_spec_role_fn_(spec_nav);
			auto type = get_spec_type_fn_(spec_nav);
			auto defn = get_agent_defn(*role, *type);
			return defn->num_effectors_fn()(spec_nav);
		});
		get_inst_num_effectors_fn_.add_dependency(get_inst_spec_nav_fn_);
		get_inst_num_effectors_fn_.add_dependency(get_spec_role_fn_);
		get_inst_num_effectors_fn_.add_dependency(get_spec_type_fn_);
		for(auto const& role_entry : m_agent_defns)
		{
			for(auto const& type_entry : *role_entry.second)
			{
				get_inst_num_effectors_fn_.add_dependency(type_entry->num_effectors_fn());
			}
		}
		///////////////

		auto details_cond_fn = [this](
			std::string cond_role_name,
			std::string cond_type_name,
			ddl::ref_resolver const& rr,
			ddl::navigator nav) -> ddl::cnd::ConditionResult
		{
			// Nav to associated spec
			auto spec_nav = get_inst_spec_nav_fn_(nav);
			if(!spec_nav)
			{
				return ddl::cnd::ConditionResult::Fail;
			}

			// Compare role and spec types
			auto role_name = get_spec_role_fn_(spec_nav);
			auto type_name = get_spec_type_fn_(spec_nav);
			if(role_name == cond_role_name && type_name == cond_type_name)
			{
				return ddl::cnd::ConditionResult::Pass;
			}
			else
			{
				return ddl::cnd::ConditionResult::Fail;
			}
		};

		auto inst_details_spc = spc.conditional("inst_details_?");
		for(auto const& entry : m_agent_defns)
		{
			auto const& role_name = entry.first;
			auto const& defns = *entry.second;
			for(auto const& adefn : defns)
			{
				defn_node dn = adefn->get_instance_defn(spc);
				auto bound_fn = std::bind(details_cond_fn, role_name, adefn->get_name(), std::placeholders::_1, std::placeholders::_2);
				auto cust_cond = ddl::cnd::custom_condition{ bound_fn };
				// Register dependencies (of the above details_cond_fn)
				cust_cond.add_dependency(get_inst_spec_nav_fn_);
				cust_cond.add_dependency(get_spec_role_fn_);
				cust_cond.add_dependency(get_spec_type_fn_);
				inst_details_spc = inst_details_spc
					(ddl::spc_condition{ cust_cond, dn });
			}
		}

		defn_node inst_details = inst_details_spc;

		return spc.composite("agent_inst")(ddl::define_children{}
			("spec_reference", spec_ref)
			("inst_details", inst_details)
			);
	}

	ddl::defn_node basic_system_defn::get_agent_controller_defn(ddl::specifier& spc)
	{
		auto get_all_controller_classes_fn = ddl::enum_defn_node::enum_values_str_fn_t{ [this](ddl::navigator nav)
		{
			ddl::enum_defn_node::enum_str_set_t classes;
			std::transform(
				std::begin(m_controller_defns),
				std::end(m_controller_defns),
				std::inserter(classes, std::end(classes)),
				[](controller_defn_map_t::value_type const& entry)
			{
				return entry.first;
			});
			return classes;
		}
		};

		ddl::defn_node con_class = spc.enumeration("controller_class")
			(ddl::spc_range< size_t >{ 1, 1 })
			(ddl::define_enum_func{ get_all_controller_classes_fn })
			;
		auto rf_con_class = ddl::node_ref{ con_class };
		////////
		get_controller_class_fn_.set_fn([rf_con_class](ddl::navigator nav) -> boost::optional< std::string >
		{
			nav = ddl::nav_to_ref(rf_con_class, nav);
			if(nav)
			{
				auto vnode = nav.get();
				if(!vnode.as_enum().empty())
				{
					return vnode.as_single_enum_str();
				}
			}

			return boost::none;
		});
		get_controller_class_fn_.add_dependency(ddl::node_dependency(rf_con_class));
		////////

		auto get_all_controller_types_fn = ddl::enum_defn_node::enum_values_str_fn_t{ [this, rf_con_class](ddl::navigator nav)
		{
			nav = ddl::nav_to_ref(rf_con_class, nav);
			ddl::enum_defn_node::enum_str_set_t type_names;
			auto class_name = nav.get().as_single_enum_str();
			auto const& controllers = *m_controller_defns.at(class_name);
			for(auto const& c : controllers)
			{
				type_names.insert(std::end(type_names), c->get_name());
			}
			return type_names;
		}
		};
		// TODO: Need a nice way to ensure that any node reference captured and used in the lambda function
		// is also registered as a dependency
		get_all_controller_types_fn.add_dependency(ddl::node_dependency(rf_con_class));

		ddl::defn_node controller_type = spc.enumeration("controller_type")
			(ddl::spc_range< size_t >{ 1, 1 })
			(ddl::define_enum_func{ get_all_controller_types_fn })
			;
		auto rf_controller_type = ddl::node_ref{ controller_type };
		////////
		get_controller_type_fn_.set_fn([rf_controller_type](ddl::navigator nav) -> boost::optional< std::string >
		{
			nav = ddl::nav_to_ref(rf_controller_type, nav);
			if(nav)
			{
				auto vnode = nav.get();
				if(!vnode.as_enum().empty())
				{
					return vnode.as_single_enum_str();
				}
			}

			return boost::none;
		});
		get_controller_type_fn_.add_dependency(ddl::node_dependency(rf_controller_type));
		////////

		auto controller_details_spc = spc.conditional("controller_details_?");
		for(auto const& entry : m_controller_defns)
		{
			auto const& cls_name = entry.first;
			auto const& defns = *entry.second;
			for(auto const& cdefn : defns)
			{
				ddl::defn_node dn = cdefn->get_defn(spc);
				controller_details_spc = controller_details_spc
					// TODO: just checking agent type name, should really check role also!?
					(ddl::spc_condition{ ddl::cnd::equal{ rf_controller_type, cdefn->get_name() }, dn });
			}
		}

		ddl::defn_node controller_details = controller_details_spc;

		ddl::defn_node controller_contents = spc.composite("controller_contents")(ddl::define_children{}
			("details", controller_details)
			);

		ddl::defn_node class_contents = spc.composite("class_contents")(ddl::define_children{}
			("controller_type", controller_type)
			("controller_contents", spc.conditional("controller_contents_?")(ddl::spc_condition{ ddl::cnd::is_selection{ rf_controller_type }, controller_contents }))
			);

		return spc.composite("controller")(ddl::define_children{}
			("controller_class", con_class)
			("class_contents", spc.conditional("class_contents_?")(ddl::spc_condition{ ddl::cnd::is_selection{ rf_con_class }, class_contents }))
			);

		/*
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
		*/
	}

	ddl::defn_node basic_system_defn::get_agent_specification_list_defn(ddl::specifier& spc)
	{
		return spc.list("spec_list")
			(ddl::spc_item{ get_agent_specification_defn(spc) })
			;
	}

	ddl::defn_node basic_system_defn::get_agent_instance_list_defn(ddl::specifier& spc)
	{
		// NOTE: Currently order here is important, due to way dep functions are copied by value
		// and so must be initialized before being passed.
		ddl::defn_node inst_defn = get_agent_instance_defn(spc);
		ddl::defn_node cont_defn = get_agent_controller_defn(spc);

		ddl::defn_node inst_and_controller = spc.composite("inst_&_controller")(ddl::define_children{}
			("inst_contents", inst_defn)
			("inst_controller", cont_defn)
			);

		return spc.list("inst_list")
			(ddl::spc_item{ inst_and_controller })
			;
	}


	state_value_id_list basic_system_defn::get_system_state_values(ddl::navigator nav) const
	{
		return get_system_state_values_fn_(nav);
	}

	ddl::dep_function< state_value_id_list > basic_system_defn::get_system_state_values_fn() const
	{
		return get_system_state_values_fn_;
	}

	std::vector< std::string > basic_system_defn::get_agent_type_names() const
	{
		// TODO: Provide role as argument?
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
	/*
	std::vector< std::string > basic_system_defn::get_agent_sensor_names(prm::param agent_type, prm::param_accessor param_vals) const
	{
		// TODO: through virtual of i_agent_defn
		return{};
	}
	*/

/*	size_t basic_system_defn::get_agent_num_effectors(ddl::navigator spec_nav) const
	{
		auto role = get_spec_role_fn_(spec_nav);
		auto type = get_spec_type_fn_(spec_nav);
		auto defn = get_agent_defn(*role, *type);
		return defn->num_effectors(spec_nav);
	}
	*/

	ddl::dep_function< size_t > basic_system_defn::get_inst_num_effectors_fn() const
	{
		return get_inst_num_effectors_fn_;
	}

	ddl::dep_function< std::vector< ddl::navigator > >
		basic_system_defn::get_connected_spec_navs_fn(std::string controller_cls, std::string controller_type) const
	{
		auto fn = get_connected_spec_navs_fn_;
		fn.set_fn(std::bind(
			get_connected_spec_navs_fn_impl_,
			controller_cls,
			controller_type,
			std::placeholders::_1
			));
		return fn;
	}

#if 0
	boost::optional< std::string > basic_system_defn::get_agent_instance_spec_name(ddl::navigator nav) const
	{
		nav = ddl::nav_to_ref(ddl::node_ref{ dn_inst_spec_ref_ }, nav);
		if(nav)
		{
			auto node = nav.get();
			if(!node.as_enum().empty())
			{
				return node.as_single_enum_str();
			}
		}

		return boost::none;
	}
#endif

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

	system_ptr basic_system_defn::create_system(ddl::navigator nav) const
	{
		//auto controller = create_controller(acc);
		//sys->register_agent_controller(0, std::move(controller));

		auto sys = create_system_core(nav["core"]);

		auto inst_list_nav = nav["instances"];
		auto inst_count = inst_list_nav.list_num_items();
		for(size_t i_inst = 0; i_inst < inst_count; ++i_inst)
		{
			auto inst_nav = inst_list_nav[i_inst];
			auto spec_nav = get_inst_spec_nav_fn_(inst_nav);
			auto role = get_spec_role_fn_(spec_nav);
			auto type = get_spec_type_fn_(spec_nav);
			auto agent_defn = get_agent_defn(*role, *type);

			auto agent = agent_defn->create_agent(
				spec_nav["details"][(size_t)0],
				inst_nav["inst_contents"]["inst_details"][(size_t)0]);
			auto agent_id = sys->register_agent(std::move(agent));
			// TODO: Maybe not necessary. Could leave to system implementation to do within register_agent()
			//static_cast<basic_agent*>(agent.get())->create(sys.get());

			auto con_nav = inst_nav["inst_controller"];
			auto con_cls = get_controller_class_fn_(con_nav);
			auto con_type = get_controller_type_fn_(con_nav);

			auto con_defn = get_controller_defn(*con_cls, *con_type);
			auto controller = con_defn->create_controller(
				con_nav["class_contents"][(size_t)0]["controller_contents"][(size_t)0]["details"][(size_t)0]);

			sys->register_agent_controller(agent_id, std::move(controller));
		}

#if 0
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
#endif
		return std::move(sys);
	}

}



