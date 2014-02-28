// rtp_properties.h

#ifndef __NE_RTP_PROPERTIES_H
#define __NE_RTP_PROPERTIES_H

#include <boost/any.hpp>

#include <string>
#include <vector>
#include <map>

// TODO: Provide some type information??

class i_properties
{
public:
	virtual size_t num_properties() const = 0;
	virtual std::string get_property_name(size_t index) const = 0;
};

class i_property_values
{
public:
	virtual boost::any get_property(std::string const& prop_name) const = 0;
};

/*
struct rtp_properties_pair
{
	i_properties* props;
	i_property_values* vals;

	rtp_properties_pair(i_properties* _props = nullptr, i_property_values* _vals = nullptr): props(_props), vals(_vals)
	{}
};
*/

class rtp_stored_properties: public i_properties
{
public:
	virtual size_t num_properties() const
	{
		return m_prop_names.size();
	}

	virtual std::string get_property_name(size_t index) const
	{
		return m_prop_names[index];
	}

public:
	void add_property(std::string const& name)
	{
		m_prop_names.push_back(name);
	}

private:
	std::vector< std::string > m_prop_names;
};

class rtp_stored_property_values: public i_property_values
{
public:
	virtual boost::any get_property(std::string const& prop_name) const
	{
		auto it = m_values.find(prop_name);
		if(it == m_values.end())
		{
			return boost::any();
		}
		else
		{
			return it->second;
		}
	}

public:
	void set_value(std::string const& prop_name, boost::any const& val)
	{
		m_values[prop_name] = val;
	}

private:
	std::map< std::string, boost::any > m_values;
};



#endif


