// system_state_values.cpp

#include "system_state_values.h"

#include <regex>


namespace sys {

	state_value_id::state_value_id()
	{

	}

	state_value_id::state_value_id(std::string name):
		m_components{ 1u, name }
	{

	}

	state_value_id& state_value_id::operator += (std::string const& rhs)
	{
		m_components.push_back(rhs);
		return *this;
	}

	state_value_id state_value_id::operator + (std::string const& rhs)
	{
		state_value_id svid{ *this };
		svid += rhs;
		return svid;
	}

	void state_value_id::pop()
	{
		m_components.pop_back();
	}

	std::string state_value_id::root() const
	{
		return m_components.front();
	}
	
	state_value_id state_value_id::tail() const
	{
		state_value_id t;
		t.m_components = std::vector < std::string > {
			std::next(std::begin(m_components)),
			std::end(m_components)
		};
		return t;
	}
	
	std::string state_value_id::to_string() const
	{
		std::string str;
		for(auto c : m_components)
		{
			if(!str.empty())
			{
				str += '/';
			}
			str += c;
		}
		return str;
	}

	state_value_id state_value_id::from_string(std::string const& str)
	{
		std::regex rgx("/");
		state_value_id svid;
		svid.m_components = std::vector< std::string >{
			std::sregex_token_iterator(std::begin(str), std::end(str), rgx, -1),
			std::sregex_token_iterator()
		};
		return svid;
	}

	bool state_value_id::operator< (state_value_id const& rhs) const
	{
		auto const end = std::end(m_components);
		auto const r_end = std::end(rhs.m_components);
		for(
			auto it = std::begin(m_components), r_it = std::begin(rhs.m_components);
			it != end && r_it != r_end;
		++it, ++r_it
			)
		{
			if(*it < *r_it)
			{
				return true;
			}
			else if(*it > *r_it)
			{
				return false;
			}
		}

		if(m_components.size() < rhs.m_components.size())
		{
			return true;
		}
		else
		{
			return false;
		}
	}

}


