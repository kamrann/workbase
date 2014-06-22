// rtp_phys_controller.cpp

#include "rtp_phys_controller.h"
#include "controllers/passive.h"
#include "controllers/interactive.h"

#include "wt_param_widgets/pw_yaml.h"


namespace rtp_phys
{
	std::string const i_phys_controller::Names[] = {
		"Passive",
		"Interactive",
	};
}

namespace YAML {
	template <>
	struct convert< rtp_phys::i_phys_controller::Type >
	{
		static Node encode(rtp_phys::i_phys_controller::Type const& rhs)
		{
			return Node(rtp_phys::i_phys_controller::Names[rhs]);
		}

		static bool decode(Node const& node, rtp_phys::i_phys_controller::Type& rhs)
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

		static std::map< std::string, rtp_phys::i_phys_controller::Type > const mapping_;
	};

	std::map< std::string, rtp_phys::i_phys_controller::Type > const convert< rtp_phys::i_phys_controller::Type >::mapping_ = {
		{ "Passive", rtp_phys::i_phys_controller::Type::Passive },
		{ "Interactive", rtp_phys::i_phys_controller::Type::Interactive },
	};
}

namespace rtp_phys {

	i_phys_controller::enum_param_type::enum_param_type()
	{
		for(size_t i = 0; i < Type::Count; ++i)
		{
			add_item(Names[i], (Type)i);
		}
		set_default_index(0);
	}

	rtp_param_type* i_phys_controller::params(Type type)
	{
		switch(type)
		{
			case i_phys_controller::Passive:
			// TODO:
			return new rtp_staticparamlist_param_type(rtp_named_param_list());

			case i_phys_controller::Interactive:
			// TODO:
			return new rtp_staticparamlist_param_type(rtp_named_param_list());

			default:
			return nullptr;
		}
	}

	namespace sb = prm::schema;

	YAML::Node i_phys_controller::get_schema(YAML::Node const& param_vals)
	{
		return sb::enum_selection("Controller Type", { begin(Names), end(Names) });
	}

	i_phys_controller* i_phys_controller::create_instance(Type type, rtp_param param)
	{
		switch(type)
		{
			case Passive:
			return new passive_controller(/*sub*/);

			case Interactive:
			return new interactive_controller(/*sub*/);

			default:
			return nullptr;
		}
	}

	i_phys_controller* i_phys_controller::create_instance(YAML::Node const& param)
	{
		auto type = prm::find_value(param, "Controller Type").as< Type >();
		switch(type)
		{
			case Passive:
			return new passive_controller(/*sub*/);

			case Interactive:
			return new interactive_controller(/*sub*/);

			default:
			return nullptr;
		}
	}
}


