// rtp_system.h

#ifndef __NE_RTP_SYSTEM_H
#define __NE_RTP_SYSTEM_H

#include "../rtp_defs.h"
#include "rtp_system_agent_specification.h"
#include "rtp_system_state_values.h"

//
#include "../rtp_observer.h"
//

#include "wt_param_widgets/par_wgt.h"

#include <boost/optional.hpp>

#include <array>
#include <string>
#include <memory>
//#ifdef PERF_LOGGING
#include <chrono>
//#endif


namespace Wt {
	class WWidget;
	class WComboBox;
}

namespace rtp {

	struct _SystemType
	{
		enum SystemType {
			//	NoughtsAndCrosses,
			//	ShipAndThrusters2D,
			Physics2D,
			Elevator,

			Count,
			None = Count,
			Default = None,
		};
	};
	typedef _SystemType::SystemType SystemType;

	extern const std::array< std::string, SystemType::Count > SystemNames;


	class i_agent;
	class i_controller;
	class i_observer;
	class i_population_wide_observer;
	class i_genome_mapping;
	class i_agent_factory;
	class i_controller_factory;
	class i_properties;
	class i_property_values;
	class i_system_drawer;
	class interactive_input;
	class i_system_factory;

	struct agents_data;

	class i_system
	{
	public:
		static std::string update_schema_providor(prm::schema::schema_provider_map_handle provider, prm::qualified_path const& prefix, bool evolvable);
		static std::unique_ptr< i_system_factory > generate_system_factory(prm::param_accessor param_vals);
		static agents_data generate_agents_data(prm::param_accessor param_vals, bool evolvable);

	public:
		static std::vector< std::string > get_agent_type_names(SystemType sys_type);
		static std::vector< std::string > get_agent_sensor_names(SystemType sys_type, prm::param agent_type, prm::param_accessor param_valss);
		static size_t get_agent_num_effectors(SystemType sys_type, prm::param agent_type);
		static std::vector< std::string > get_system_state_values(prm::param_accessor param_vals);

	public:
		struct update_info
		{
			enum class Type {
				Realtime,
				Nonrealtime,
			};

			Type type;
			double frequency;	// Updates per second, for realtime systems

			update_info(): type(Type::Nonrealtime), frequency(0.0) {}
		};

	public:
#if 0
		virtual boost::any generate_initial_state() const = 0;
		virtual void set_state(boost::any const& st) = 0;
#endif

		virtual void initialize() = 0;
		virtual void clear_agents() = 0;

		virtual update_info get_update_info() const = 0;

		typedef unsigned int agent_id;
		virtual agent_id register_agent(agent_specification const& spec) = 0;// , std::unique_ptr< i_controller >&& controller) = 0;
		virtual void register_agent_controller(agent_id agent, std::unique_ptr< i_controller > controller) = 0;

		virtual i_agent const& get_agent(agent_id id) const = 0;
		virtual i_controller const& get_agent_controller(agent_id id) const = 0;

		virtual void register_interactive_input(interactive_input const& input) = 0;
		virtual bool update(i_observer* obs = nullptr) = 0;
		virtual i_observer::observations_t record_observations(i_observer* obs) const = 0;

		virtual std::shared_ptr< i_properties const > get_state_properties() const = 0;
		virtual std::shared_ptr< i_property_values const > get_state_property_values() const = 0;

		virtual double get_state_value(state_value_id id) const = 0;

		virtual std::unique_ptr< i_system_drawer > get_drawer() const = 0;

		void set_random_seed(unsigned long rseed);

//#ifdef PERF_LOGGING
		typedef std::map< std::string, std::chrono::duration< double > > perf_data_t;
		virtual void concatenate_performance_data(perf_data_t& pd) const = 0;
		static void output_performance_data(perf_data_t const& pd, std::ostream& strm);
//#endif

	public:
		virtual ~i_system()
		{}

	protected:
		rgen_t m_rgen;
	};


	class i_system_factory
	{
	public:
		virtual std::unique_ptr< i_system > create_system() const = 0;
		virtual void set_random_seed(uint32_t seed) = 0;

	public:
		virtual ~i_system_factory() {}
	};

	class system_factory: public i_system_factory
	{
	public:
		typedef std::function< std::unique_ptr< i_system >() > create_fn_t;

		system_factory(create_fn_t fn): m_fn(fn), m_fixed_random(boost::none)
		{}

		virtual std::unique_ptr< i_system > create_system() const override
		{
			auto sys = m_fn();
			auto seed = m_fixed_random ?
				*m_fixed_random :
				static_cast<uint32_t>(std::chrono::high_resolution_clock::now().time_since_epoch().count() & 0xffffffff);
			sys->set_random_seed(seed);
			return std::move(sys);
		}

		virtual void set_random_seed(uint32_t seed) override
		{
			m_fixed_random = seed;
		}

	private:
		create_fn_t m_fn;

		// If this is set, all generated systems will have this same random seed.
		// If unset, every generated system will be initialized with a randomly chosen seed (default).
		boost::optional< uint32_t > m_fixed_random;
	};
}


#include <yaml-cpp/yaml.h>

namespace YAML {

	template <>
	struct convert< rtp::SystemType >
	{
		static Node encode(rtp::SystemType const& rhs)
		{
			return Node(rtp::SystemNames[rhs]);
		}

		static bool decode(Node const& node, rtp::SystemType& rhs)
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

		static std::map< std::string, rtp::SystemType > const mapping_;
	};
}


#endif

