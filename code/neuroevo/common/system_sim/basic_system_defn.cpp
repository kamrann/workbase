// basic_system_defn.cpp

#include "basic_system_defn.h"
#include "controller_defn.h"
#include "rtp_controller.h"

#include "params/param.h"
#include "params/schema_builder.h"
#include "params/param_accessor.h"


namespace sys {

	void basic_system_defn::add_controller_defn(std::string controller_class, std::unique_ptr< i_controller_defn > defn)
	{
		auto mp_it = m_controller_defns.insert(controller_defn_map_t::value_type{
			controller_class,
			std::make_shared< controller_defn_list_t >()
		}).first;

		mp_it->second->push_back(std::move(defn));
	}

	namespace sb = prm::schema;

	std::string basic_system_defn::update_schema_provider_for_agent_controller(prm::schema::schema_provider_map_handle provider, prm::qualified_path const& prefix) const
	{
		auto relative = std::string{ "agent_controller" };
		auto path = prefix + relative;

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
				// NOTE: Ignoring return value and relying on defn using its name as its relative path
				// TODO: This doesn't really fit with general approach...
				// Might be better for caller to always provide path name to update_schema_provider()?
				cdefn->update_schema_providor(provider, path);
			}
		}

		(*provider)[path] = [=](prm::param_accessor acc)
		{
			auto s = sb::list("ctype_contents");
			if(acc.is_available("controller_type") && !prm::is_unspecified(acc["controller_type"]))
			{
				auto controller_name = prm::extract_as< prm::enum_param_val >(acc["controller_type"])[0];
				auto contents_path =
					path +
					controller_name;
				sb::append(s, provider->at(contents_path)(acc));
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
			auto s = sb::list(relative);
			sb::append(s, provider->at(path + std::string("controller_class"))(acc));
			sb::append(s, provider->at(path + std::string("cclass_contents"))(acc));

			sb::border(s, std::string{ "Controller" });
			return s;
		};

		return relative;
	}


	controller_ptr basic_system_defn::create_controller(prm::param_accessor acc) const
	{
		auto cls = prm::extract_as< prm::enum_param_val >(acc["controller_class"])[0];
		auto type = prm::extract_as< prm::enum_param_val >(acc["controller_type"])[0];

		auto& cls_defns = *m_controller_defns.at(cls);
		auto c_it = std::find_if(
			std::begin(cls_defns),
			std::end(cls_defns),
			[type](std::unique_ptr< i_controller_defn > const& con)
		{
			return con->get_name() == type;
		});
		return (*c_it)->create_controller();	// TODO: should take acc ??
	}

}



