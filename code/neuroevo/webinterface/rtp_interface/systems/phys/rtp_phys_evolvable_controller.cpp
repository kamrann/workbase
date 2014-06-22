// rtp_phys_evolvable_controller.cpp

#include "rtp_phys_evolvable_controller.h"
#include "controllers/mlp.h"

#include "wt_param_widgets/pw_yaml.h"


namespace rtp_phys
{
	std::string const evolvable_controller::Names[] = {
		"MLP",
	};
}

namespace YAML {
	template <>
	struct convert< rtp_phys::evolvable_controller::Type >
	{
		static Node encode(rtp_phys::evolvable_controller::Type const& rhs)
		{
			return Node(rtp_phys::evolvable_controller::Names[rhs]);
		}

		static bool decode(Node const& node, rtp_phys::evolvable_controller::Type& rhs)
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

		static std::map< std::string, rtp_phys::evolvable_controller::Type > const mapping_;
	};

	std::map< std::string, rtp_phys::evolvable_controller::Type > const convert< rtp_phys::evolvable_controller::Type >::mapping_ = {
		{ "MLP", rtp_phys::evolvable_controller::Type::MLP },
	};
}


namespace rtp_phys {
	
	evolvable_controller::enum_param_type::enum_param_type()
	{
		for(size_t i = 0; i < Type::Count; ++i)
		{
			add_item(Names[i], (Type)i);
		}
		set_default_index(0);
	}

	rtp_param_type* evolvable_controller::params(Type ea_type)
	{
		switch(ea_type)
		{
			case MLP:
			return new mlp_controller::param_type();

			default:
			assert(false);
			return nullptr;
		}
	}

	namespace sb = prm::schema;

	YAML::Node evolvable_controller::get_schema(YAML::Node const& param_vals)
	{
		auto schema = sb::list("evolvable_controller");

		auto type = sb::enum_selection("Controller Type", { begin(Names), end(Names) });
		sb::on_update(type);
		sb::append(schema, type);

		auto node = prm::find_value(param_vals, "Controller Type");
		auto enum_sel = node.IsNull() ? Type::Default : node.as< Type >();
		switch(enum_sel)
		{
			case Type::MLP:
			{
				sb::append(schema, mlp_controller::get_schema(param_vals));
			}
			break;

			default:
			assert(false);
		}

		return schema;
	}

	std::tuple< i_genome_mapping*, i_agent_factory* > evolvable_controller::create_instance(Type type, rtp_param param)
	{
		switch(type)
		{
			case MLP:
			return mlp_controller::create_instance_evolvable(param);

			default:
			assert(false);
			return std::tuple< i_genome_mapping*, i_agent_factory* >(nullptr, nullptr);
		}
	}

	std::tuple< i_genome_mapping*, i_agent_factory* > evolvable_controller::create_instance(YAML::Node const& param)
	{
		auto type = prm::find_value(param, "Controller Type").as< Type >();
		switch(type)
		{
			case MLP:
			return mlp_controller::create_instance_evolvable(param);

			default:
			assert(false);
			return std::tuple< i_genome_mapping*, i_agent_factory* >(nullptr, nullptr);
		}
	}

}


