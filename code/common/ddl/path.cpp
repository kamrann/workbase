// path.cpp

#include "path.h"


namespace ddl {

	path::path()
	{

	}

	void path::append(std::string str)
	{
		push_component(std::move(str));
	}
		
	void path::append(unsigned int idx)
	{
		push_component(idx);
	}
		
	void path::pop()
	{
		components_.pop_back();
	}
		
	void path::reset()
	{
		components_.clear();
	}

	size_t path::length() const
	{
		return components_.size();
	}
		
	std::string path::to_string(int idx_base) const
	{
		std::string result;
		for(auto const& c : components_)
		{
			result += ">";
			result += c.to_string(idx_base);
		}
		return result;
	}

	bool path::operator< (path const& rhs) const
	{
		auto count = length();
		auto rhs_count = rhs.length();
		auto common_count = std::min(count, rhs_count);
		for(size_t i = 0; i < common_count; ++i)
		{
			auto const& comp = components_[i];
			auto const& rhs_comp = rhs.components_[i];
			if(comp < rhs_comp)
			{
				return true;
			}
			else if(rhs_comp < comp)
			{
				return false;
			}
		}

		return count < rhs_count;
	}
		
	bool path::operator== (path const& rhs) const
	{
		return length() == rhs.length() &&
			std::equal(
			std::begin(components_), std::end(components_),
			std::begin(rhs.components_)
			);
	}
		
	path& path::operator+= (std::string str)
	{
		append(str);
		return *this;
	}
		
	path& path::operator+= (unsigned int idx)
	{
		append(idx);
		return *this;
	}
		
/*	path::operator bool() const
	{
		return
	}
		
	bool path::operator! () const
	{

	}
*/

	path::component::component()
	{

	}

	path::component::component(std::string str):
		var_{ str }
	{

	}

	path::component::component(unsigned int idx):
		var_{ idx }
	{

	}

	path::ComponentType path::component::type() const
	{
		return (ComponentType)var_.which();
	}

	std::string path::component::as_child() const
	{
		return boost::get< std::string >(var_);
	}

	unsigned int path::component::as_index() const
	{
		return boost::get< unsigned int >(var_);
	}

	std::string path::component::to_string(int idx_base) const
	{
		switch(type())
		{
			case ComponentType::CompositeChild:
			return as_child();

			case ComponentType::ListItem:
			{
				std::string result;
				result += "[";
				result += std::to_string(idx_base + as_index());
				result += "]";
				return result;
			}
		}
	}

	path path::common_ancestor(path const& p1, path const& p2)
	{
		path common;
		for(auto it1 = std::begin(p1), it2 = std::begin(p2);
			it1 != std::end(p1) && it2 != std::end(p2) && *it1 == *it2; ++it1, ++it2)
		{
			common.push_component(*it1);
		}
		return common;
	}


	bool path::component::operator< (component const& rhs) const
	{
		auto this_type = type();
		auto rhs_type = rhs.type();
		if(this_type < rhs_type)
		{
			return true;
		}
		else if(this_type > rhs_type)
		{
			return false;
		}
		else switch(this_type)
		{
			case ComponentType::CompositeChild:
			return as_child() < rhs.as_child();

			case ComponentType::ListItem:
			return as_index() < rhs.as_index();
		}
	}

	bool path::component::operator== (component const& rhs) const
	{
		return !(*this < rhs || rhs < *this);
	}

	void path::push_component(component c)
	{
		components_.push_back(std::move(c));
	}

	path::component_list::const_iterator path::begin() const
	{
		return std::begin(components_);
	}

	path::component_list::const_iterator path::end() const
	{
		return std::end(components_);
	}

}




