// rtp_system.cpp

#include "rtp_system.h"
#include "../rtp_param_widget.h"
#include "../params/paramlist_par.h"
#include "../params/dimensionality_par.h"
#include "../rtp_param_manager.h"

//#include "nac/rtp_nac_system.h"
//#include "sat/rtp_sat_system.h"
#include "phys/rtp_phys_system.h"

#include "wt_param_widgets/pw_yaml.h"

#include <Wt/WComboBox>
#include <Wt/WContainerWidget>


const std::array< std::string, NumSystems > SystemNames = {
//	"Noughts & Crosses",
//	"Ship & Thrusters 2D",
	"Physics 2D",
};


namespace YAML {
	template <>
	struct convert< SystemType >
	{
		static Node encode(SystemType const& rhs)
		{
			return Node(SystemNames[rhs]);
		}

		static bool decode(Node const& node, SystemType& rhs)
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

		static std::map< std::string, SystemType > const mapping_;
	};

	std::map< std::string, SystemType > const convert< SystemType >::mapping_ = {
		{ "Physics 2D", SystemType::Physics2D },
	};
}


system_type_param_type::system_type_param_type()
{
	for(size_t i = 0; i < NumSystems; ++i)
	{
		add_item(SystemNames[i], (SystemType)i);
	}
	set_default_index(1);
}


system_param_type::system_param_type(bool evolvable)
{
	m_evolvable = evolvable;
	add_dependency("System Type", 1);
}

size_t system_param_type::provide_num_child_params(rtp_param_manager* mgr) const
{
	return 2;
}

rtp_named_param system_param_type::provide_child_param(size_t index, rtp_param_manager* mgr) const
{
	switch(index)
	{
		case 0:
		return rtp_named_param(new system_type_param_type(), "System Type");

		case 1:
		{
			SystemType system = boost::any_cast<SystemType>(mgr->retrieve_param("System Type"));
			return i_system::params(system, m_evolvable);
		}

		default:
		return rtp_named_param();
	}
}


rtp_param_type* i_system::params(bool evolvable)
{
	return new system_param_type(evolvable);
}

rtp_param_type* i_system::params(SystemType sys, bool evolvable)
{
	switch(sys)
	{
/*		case NoughtsAndCrosses:
		// TODO:
		return new rtp_staticparamlist_param_type(rtp_named_param_list());

		case ShipAndThrusters2D:
		return rtp_sat::sat_system< WorldDimensionality::dim2D >::params(evolvable);
*/
		case Physics2D:
		return rtp_phys::phys_system::params(evolvable);

		default:
		return nullptr;
	}
}

namespace sb = prm::schema;

YAML::Node i_system::get_schema(YAML::Node const& param_vals, bool evolvable)
{
	auto schema = sb::list("sys_params");
	sb::label(schema, "System Params");

	auto type = sb::enum_selection("System Type", { begin(SystemNames), end(SystemNames) });
	sb::on_update(type);
	sb::append(schema, type);

	auto node = prm::find_value(param_vals, "System Type");
	auto enum_sel = node.IsNull() ? SystemType::Default : node.as< SystemType >();
	switch(enum_sel)
	{
		case SystemType::Physics2D:
		{
			sb::append(schema, rtp_phys::phys_system::get_schema(param_vals, evolvable));
		}
		break;

		default:
		assert(false);
	}

	return schema;
}

std::tuple< i_system*, i_genome_mapping*, i_agent_factory*, i_observer*, i_population_wide_observer* > i_system::create_instance(rtp_param param, bool evolvable)
{
	auto param_list = boost::any_cast<rtp_param_list>(param);
	SystemType type = boost::any_cast<SystemType>(param_list[0]);
	switch(type)
	{
/*		case NoughtsAndCrosses:
		return std::tuple< i_system*, i_genome_mapping*, i_agent_factory*, i_observer* >(nullptr, nullptr, nullptr, nullptr);
	
		case ShipAndThrusters2D:
		return rtp_sat::sat_system< WorldDimensionality::dim2D >::create_instance(param_list[1], evolvable);
*/	
		case Physics2D:
		return rtp_phys::phys_system::create_instance(param_list[1], evolvable);
	
		default:
		return std::tuple< i_system*, i_genome_mapping*, i_agent_factory*, i_observer*, i_population_wide_observer* >(nullptr, nullptr, nullptr, nullptr, nullptr);
	}
}

std::tuple< i_system*, i_genome_mapping*, i_agent_factory*, i_observer*, i_population_wide_observer* > i_system::create_instance(YAML::Node const& param, bool evolvable)
{
	SystemType type = prm::find_value(param, "System Type").as< SystemType >();
	switch(type)
	{
		/*		case NoughtsAndCrosses:
		return std::tuple< i_system*, i_genome_mapping*, i_agent_factory*, i_observer* >(nullptr, nullptr, nullptr, nullptr);

		case ShipAndThrusters2D:
		return rtp_sat::sat_system< WorldDimensionality::dim2D >::create_instance(param_list[1], evolvable);
		*/
		case Physics2D:
		return rtp_phys::phys_system::create_instance(param, evolvable);

		default:
		return std::tuple< i_system*, i_genome_mapping*, i_agent_factory*, i_observer*, i_population_wide_observer* >(nullptr, nullptr, nullptr, nullptr, nullptr);
	}
}

