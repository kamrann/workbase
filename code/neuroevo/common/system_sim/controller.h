// controller.h

#ifndef __NE_SYSSIM_CONTROLLER_H
#define __NE_SYSSIM_CONTROLLER_H

#include "system_state_values.h"

//#include "params/param_fwd.h"

#include "util/bimap.h"

#include <vector>
#include <memory>
#include <functional>


namespace sys {

	class i_agent;
	class i_controller_factory;

	class i_controller
	{
	public:
		enum Type {
//			Evolved,

			Passive,
			Interactive,
//			Database,
			Preset,

			Count,
			None = Count,
			Default = None,
		};

//		static std::string const Names[Type::Count];
		static bimap< Type, std::string > const TypeMp;

		static Type decode_type(std::string const& as_str);

//		static prm::param get_controller_agent_type(prm::param_accessor param);
//		static std::string update_schema_providor(prm::schema::schema_provider_map_handle provider, prm::qualified_path const& prefix);
//		static std::unique_ptr< i_controller_factory > create_factory(prm::param_accessor param);

	public:
//		typedef int input_id;
//		typedef std::vector< input_id > input_id_list_t;
		typedef state_value_id_list input_id_list_t;

		typedef double input_t;
		typedef std::vector< input_t > input_list_t;
		typedef double output_t;
		typedef std::vector< output_t > output_list_t;

		// TODO: this method is essentially just being used to create a 2 stage controller construction.
		// basically its inputs will be bound, resulting in a function which sets an array of values 
		// corresponding to the ids. this function will then be stored alongside (or perhaps within?)
		// the controller. so in fact, the bounding could be done at creation time, removing the need for this
		// method. would require the system to be constructed and available at the point the controller
		// gets created.
		virtual input_id_list_t get_input_ids() const = 0;
		virtual output_list_t process(input_list_t const& inputs) = 0;

	public:
		virtual ~i_controller() {}
	};

	class i_controller_factory
	{
	public:
		virtual ~i_controller_factory() {}

		virtual std::unique_ptr< i_controller > create(i_agent const*) const = 0;
		virtual bool is_interactive() const = 0;
	};

	class controller_factory: public i_controller_factory
	{
	public:
		typedef std::function< std::unique_ptr< i_controller >(i_agent const*) > create_fn_t;

		controller_factory(create_fn_t fn, bool interactive = false): m_fn(fn), m_interactive(interactive)
		{}

		virtual std::unique_ptr< i_controller > create(i_agent const* attached_agent) const override
		{
			return m_fn(attached_agent);
		}

		virtual bool is_interactive() const override
		{
			return m_interactive;
		}

	private:
		create_fn_t m_fn;
		bool m_interactive;
	};
}

#if 0
#include <yaml-cpp/yaml.h>

namespace YAML {

	template <>
	struct convert< sys::i_controller::Type >
	{
		static Node encode(sys::i_controller::Type const& rhs)
		{
			return Node(sys::i_controller::Names[rhs]);
		}

		static bool decode(Node const& node, sys::i_controller::Type& rhs)
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

		static std::map< std::string, sys::i_controller::Type > const mapping_;
	};
}
#endif


#endif

