// system_state_values.h

#ifndef __NE_SYSSIM_SYSTEM_STATE_VALUES_H
#define __NE_SYSSIM_SYSTEM_STATE_VALUES_H

#include <yaml-cpp/yaml.h>

#include <string>
#include <vector>
#include <map>
#include <functional>


namespace sys {

	class state_value_id
	{
	public:
		state_value_id();
		state_value_id(std::string name);

		state_value_id& operator += (std::string const& rhs);
		state_value_id operator + (std::string const& rhs);
		void pop();

		std::string root() const;
		state_value_id tail() const;

		std::string to_string() const;
		static state_value_id from_string(std::string const& str);

		bool operator< (state_value_id const& rhs) const;

	private:
		std::vector< std::string > m_components;

		friend struct YAML::convert < state_value_id > ;
	};

	typedef std::vector< state_value_id > state_value_id_list;

	typedef unsigned long state_value_bound_id;

	typedef std::function< double() > state_value_accessor_fn;

	typedef std::map< state_value_id, state_value_bound_id > sv_bindings_t;
	typedef std::vector< state_value_accessor_fn > sv_accessors_t;

}

namespace YAML {

	template <>
	struct convert< sys::state_value_id >
	{
		static Node encode(sys::state_value_id const& rhs)
		{
			Node n;
			for(auto const& c : rhs.m_components)
			{
				n.push_back(c);
			}
			return n;
		}

		static bool decode(Node const& node, sys::state_value_id& rhs)
		{
			if(!node.IsSequence())
			{
				return false;
			}

			rhs = sys::state_value_id{};
			for(Node c : node)
			{
				rhs += c.as< std::string >();
			}
			return true;
		}
	};

}


#endif

