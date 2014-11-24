// objective_value.cpp

#include "objective_value.h"


namespace ga {

	objective_value::objective_value()
	{}

	objective_value::objective_value(double val):
		_value(val)
	{

	}

	objective_value::objective_value(pareto< double > val):
		_value(val)
	{

	}

	std::string objective_value::to_string() const
	{
		std::stringstream ss;
		std::fixed(ss);
		ss.precision(2);
		// TODO: visitor maybe?
		if(_value.which() == 0)
		{
			ss << as_real();
		}
		else
		{
			auto par = as_pareto();
			for(size_t i = 0; i < par.size(); ++i)
			{
				if(i > 0)
				{
					ss << ", ";
				}

				ss << par[i];
			}
		}
		return ss.str();
	}

	YAML::Node objective_value::to_yaml() const
	{
		// TODO: visitor maybe?
		if(_value.which() == 0)
		{
			return YAML::Node{ as_real() };
		}
		else
		{
			auto node = YAML::Node{};
			auto par = as_pareto();
			for(size_t i = 0; i < par.size(); ++i)
			{
				node.push_back(par[i]);
			}
			return node;
		}
	}

}




