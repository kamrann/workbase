// system.h

#ifndef __NE_SYSSIM_SYSTEM_H
#define __NE_SYSSIM_SYSTEM_H

#include "syssim_defs.h"
#include "system_sim_fwd.h"
//#include "rtp_system_agent_specification.h"
#include "system_state_values.h"

//
#include "observer.h"
//

#include "params/param_fwd.h"

#include <boost/optional.hpp>

#include <array>
#include <vector>
#include <string>
#include <memory>
//#ifdef PERF_LOGGING
#include <chrono>
//#endif


namespace sys {

/*	struct _SystemType
	{
		enum SystemType {
			//	NoughtsAndCrosses,
			//	ShipAndThrusters2D,
			Physics2D,
			Detection2D,
			Elevator,

			Count,
			None = Count,
			Default = None,
		};
	};
	typedef _SystemType::SystemType SystemType;

	extern const std::array< std::string, SystemType::Count > SystemNames;
	*/

	class i_system
	{
	public:
#if 0
		virtual boost::any generate_initial_state() const = 0;
		virtual void set_state(boost::any const& st) = 0;
#endif

		virtual bool initialize() = 0;
		virtual void clear_agents() = 0;

		typedef unsigned int agent_id;
		virtual agent_id register_agent(agent_ptr agent) = 0;
		virtual void register_agent_controller(agent_id agent, controller_ptr controller) = 0;

		virtual i_agent const& get_agent(agent_id id) const = 0;
		virtual i_controller const& get_agent_controller(agent_id id) const = 0;

		virtual void register_interactive_input(interactive_input const& input) = 0;
		virtual bool update(i_observer* obs = nullptr) = 0;
		virtual i_observer::observations_t record_observations(i_observer* obs) const = 0;

//		virtual std::shared_ptr< i_properties const > get_state_properties() const = 0;
//		virtual std::shared_ptr< i_property_values const > get_state_property_values() const = 0;

		virtual state_value_bound_id get_state_value_binding(state_value_id svid) const = 0;
		virtual double get_state_value(state_value_bound_id bid) const = 0;

		virtual system_drawer_ptr get_drawer() const = 0;

		void set_random_seed(unsigned long rseed);
		rgen_t& get_rgen();

//#ifdef PERF_LOGGING
		typedef std::map< std::string, std::chrono::duration< double > > perf_data_t;
		virtual void concatenate_performance_data(perf_data_t& pd) const = 0;
		static void output_performance_data(perf_data_t const& pd, std::ostream& strm, size_t samples);
//#endif

	public:
		virtual ~i_system()
		{}

	protected:
		rgen_t m_rgen;
	};

	typedef std::unique_ptr< i_system > system_ptr;


#if 0
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
#endif
}

#if 0
#include <yaml-cpp/yaml.h>

namespace YAML {

	template <>
	struct convert< sys::SystemType >
	{
		static Node encode(sys::SystemType const& rhs)
		{
			return Node(sys::SystemNames[rhs]);
		}

		static bool decode(Node const& node, sys::SystemType& rhs)
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

		static std::map< std::string, sys::SystemType > const mapping_;
	};
}
#endif

#endif

