// rtp_evolvable_controller.cpp

#include "rtp_evolvable_controller.h"
#include "systems/mlp.h"
#include "rtp_genome_mapping.h"

#include "wt_param_widgets/pw_yaml.h"
#include "wt_param_widgets/schema_builder.h"
#include "wt_param_widgets/param_accessor.h"


namespace rtp
{
	std::string const evolvable_controller::Names[] = {
		"MLP",
	};
}

namespace YAML {
	template <>
	struct convert< rtp::evolvable_controller::Type >
	{
		static Node encode(rtp::evolvable_controller::Type const& rhs)
		{
			return Node(rtp::evolvable_controller::Names[rhs]);
		}

		static bool decode(Node const& node, rtp::evolvable_controller::Type& rhs)
		{
			if(!node.IsScalar())
			{
				return false;
			}

			auto it = mapping_.find(node.Scalar());
			if(it == mapping_.end())
			{
				return false;
			}

			rhs = it->second;
			return true;
		}

		static std::map< std::string, rtp::evolvable_controller::Type > const mapping_;
	};

	std::map< std::string, rtp::evolvable_controller::Type > const convert< rtp::evolvable_controller::Type >::mapping_ = {
		{ "MLP", rtp::evolvable_controller::Type::MLP },
	};
}


namespace rtp {
	
	namespace sb = prm::schema;

	std::string evolvable_controller::update_schema_providor(prm::schema::schema_provider_map_handle provider, prm::qualified_path const& prefix)
	{
		auto relative = std::string{ "evolvable_controller" };
		auto path = prefix + relative;

		(*provider)[path + std::string("ev_controller_type")] = [](prm::param_accessor)
		{
			auto s = sb::enum_selection(
				"ev_controller_type",
				std::vector< std::string >{ begin(Names), end(Names) },
				0, 1);
			sb::label(s, "Controller Type");
			sb::trigger(s, "ev_controller_type");
			return s;
		};


		path += std::string("ev_controller_type_specific_params");

		auto mlp_rel = mlp_controller::update_schema_providor(provider, path);

		(*provider)[path] = [=](prm::param_accessor param_vals)
		{
			auto s = sb::list("ev_controller_type_specific_params");
			auto node = param_vals["ev_controller_type"];
			auto enum_sel = (!node || node.as< prm::is_unspecified >()) ? Type::Default : node[0].as< Type >();
			switch(enum_sel)
			{
				case evolvable_controller::Type::MLP:
				sb::append(s, provider->at(path + mlp_rel)(param_vals));
				break;
			}
			sb::unborder(s);
			sb::update_on(s, "ev_controller_type");
			return s;
		};

		path.pop();


		(*provider)[path] = [=](prm::param_accessor param_vals)
		{
			auto s = sb::list(relative);
			sb::append(s, provider->at(path + std::string("ev_controller_type"))(param_vals));
			sb::append(s, provider->at(path + std::string("ev_controller_type_specific_params"))(param_vals));
			return s;
		};

		return relative;
	}
	
	std::tuple<
		std::unique_ptr< i_genome_mapping >,
		std::unique_ptr< i_controller_factory >
	> evolvable_controller::create_instance(prm::param_accessor param)
	{
		auto result = std::tuple< std::unique_ptr< i_genome_mapping >, std::unique_ptr< i_controller_factory > >();
		param.push_relative_path(prm::qualified_path("evolvable_controller"));
		auto type_node = param["ev_controller_type"];
		if(type_node.as< prm::is_unspecified >())
		{
			throw prm::required_unspecified("Evolvable Controller Type");
		}

		auto type = type_node[0].as< Type >();
		param.push_relative_path(prm::qualified_path("ev_controller_type_specific_params"));
		switch(type)
		{
			case MLP:
			result = mlp_controller::create_instance_evolvable(param);
			break;

			default:
			assert(false);
		}
		param.pop_relative_path();
		param.pop_relative_path();
		return result;
	}

}


