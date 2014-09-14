// qualified_path.cpp

#include "qualified_path.h"

#include <regex>


namespace prm {

	std::string const qualified_path::ScopeString{ ">" };
	std::string const qualified_path::IndexPrefix{ "[" };
	std::string const qualified_path::IndexSuffix{ "]" };

	qualified_path::component::component(std::string const& _name, boost::optional< size_t > const& _index):
		m_name(_name),
		m_index(_index)
	{

	}

	std::string const& qualified_path::component::name() const
	{
		return m_name;
	}

	bool qualified_path::component::has_index() const
	{
		return m_index;
	}

	size_t qualified_path::component::index() const
	{
		if(m_index)
		{
			return *m_index;
		}
		else
		{
			throw std::runtime_error("qualified_path has no index");
		}
	}

	std::string qualified_path::component::to_string(std::string scope_str) const
	{
		auto str = scope_str + name();
		if(has_index())
		{
			str += IndexPrefix;
			str += std::to_string(index());
			str += IndexSuffix;
		}
		return str;
	}

	void qualified_path::component::set_index(size_t idx)
	{
		m_index = idx;
	}

	bool qualified_path::component::same_name(component const& lhs, component const& rhs)
	{
		return lhs.name().compare(rhs.name()) == 0;
	}

	bool qualified_path::component::same_index(component const& lhs, component const& rhs)
	{
		bool has = lhs.has_index();
		return has == rhs.has_index() && (!has || lhs.index() == rhs.index());
	}

	bool qualified_path::component::operator== (qualified_path::component const& rhs) const
	{
		return same_name(*this, rhs) && same_index(*this, rhs);
	}

	bool qualified_path::component::operator< (qualified_path::component const& rhs) const
	{
		auto name_comp = m_name.compare(rhs.m_name);
		return name_comp < 0 || name_comp == 0 && (
			m_index < rhs.m_index
			);
	}


	qualified_path::qualified_path()
	{

	}

	qualified_path::qualified_path(std::string const& path_str)
	{
		// Tokenize with '>' as delimiter
		std::regex rgx(">");
		std::vector< std::string > comp_strings{
			std::sregex_token_iterator(std::begin(path_str), std::end(path_str), rgx, -1),
			std::sregex_token_iterator()
		};

		// For inputs beginning with '>', discard the initial empty string match
		if(!comp_strings.empty() && comp_strings.front().empty())
		{
			comp_strings.erase(comp_strings.begin());
		}

		// Iterate over tokens
		for(auto const& comp : comp_strings)
		{
			std::string name{ comp };
			boost::optional< size_t > idx = boost::none;

			// Search for index
			std::regex idx_rgx(R"(\[\d+\])");
			std::smatch m;
			if(std::regex_search(comp, m, idx_rgx))
			{
				// Index found
				assert(m.suffix().length() == 0);

				auto idx_str = m.str();
				// Strip []
				idx_str = idx_str.substr(1, idx_str.size() - 2);
				idx = std::stoul(idx_str);

				name = m.prefix();
			}

			m_components.push_back({ name, idx });
		}
	}

	std::string qualified_path::to_string(std::string scope_str, bool include_base_scope) const
	{
		std::string str;
		for(auto const& comp : m_components)
		{
			str += comp.to_string(include_base_scope || !str.empty() ? scope_str : std::string{ "" });
		}
		return str;
	}

	qualified_path qualified_path::from_string(std::string const& path_str)
	{
		return qualified_path(path_str);
	}

	size_t qualified_path::size() const
	{
		return m_components.size();
	}

	bool qualified_path::is_leaf() const
	{
		return size() == 1;
	}

	bool qualified_path::contains_indexes() const
	{
		for(auto const& c : *this)
		{
			if(c.has_index())
			{
				return true;
			}
		}
		return false;
	}

	qualified_path::component& qualified_path::root()
	{
		return m_components.front();
	}

	qualified_path::component& qualified_path::leaf()
	{
		return m_components.back();
	}

	qualified_path::component const& qualified_path::root() const
	{
		return m_components.front();
	}

	qualified_path::component const& qualified_path::leaf() const
	{
		return m_components.back();
	}

	qualified_path::const_iterator qualified_path::begin() const
	{
		return std::begin(m_components);
	}

	qualified_path::const_iterator qualified_path::end() const
	{
		return std::end(m_components);
	}

	qualified_path qualified_path::stem() const
	{
		qualified_path result;
		result.m_components.insert(
			result.begin(),
			begin(),
			begin() + size() - 1
			);
		return result;
	}

	qualified_path qualified_path::tail() const
	{
		qualified_path result;
		result.m_components.insert(
			result.begin(),
			begin() + 1,
			end()
			);
		return result;
	}

	qualified_path qualified_path::subpath(const_iterator from) const
	{
		return subpath(from, end());
	}

	qualified_path qualified_path::subpath(const_iterator from, const_iterator to) const
	{
		qualified_path sub;
		sub.m_components = component_list_t{ from, to };
		return sub;
	}


	qualified_path qualified_path::unindexed() const
	{
		qualified_path result;
		for(auto const& c : *this)
		{
			result += c.name();
		}
		return result;
	}

	qualified_path::const_iterator qualified_path::find(std::string const& unqualified) const
	{
		// TODO:
		if(m_components.back().name().compare(unqualified) == 0)
		{
			return begin() + size() - 1;
		}
		else
		{
			return end();
		}
	}

	qualified_path::const_iterator qualified_path::find_anywhere(std::string const& unqualified) const
	{
		for(auto it = begin(); it != end(); ++it)
		{
			if(it->name().compare(unqualified) == 0)
			{
				return it;
			}
		}
		return end();
	}

	bool qualified_path::is_same_or_descendent_of(qualified_path const& p) const
	{
		auto size_ = size();
		auto p_size_ = p.size();
		return size_ >= p_size_ && std::equal(begin(), begin() + p_size_, p.begin());
	}

	bool qualified_path::is_unambiguous_from(qualified_path const& source) const
	{
		auto indexed = qualified_path::indexed_after_divergence(*this, source);
		return !std::get< 0 >(indexed);
	}

	std::tuple<
		qualified_path::const_iterator,
		qualified_path::const_iterator,
		bool,
		bool
	>
	qualified_path::divergence(qualified_path const& lhs, qualified_path const& rhs)
	{
		const_iterator lhs_it = std::begin(lhs);
		const_iterator rhs_it = std::begin(rhs);
		for(
			;
			lhs_it != std::end(lhs) && rhs_it != std::end(rhs);
		++lhs_it, ++rhs_it
			)
		{
			// Are both names identical?
			if(!component::same_name(*lhs_it, *rhs_it))
			{
				// No, diverged
				return std::make_tuple(lhs_it, rhs_it, false, false);
			}
			// Different indexes?
			else if(lhs_it->has_index() && rhs_it->has_index() && lhs_it->index() != rhs_it->index())
			{
				return std::make_tuple(lhs_it, rhs_it, true, true);
			}
			// Left has index, right doesn't?
			else if(lhs_it->has_index() && !rhs_it->has_index())
			{
				return std::make_tuple(lhs_it, rhs_it, true, false);
			}
			// Right has index, left doesn't?
			else if(!lhs_it->has_index() && rhs_it->has_index())
			{
				return std::make_tuple(lhs_it, rhs_it, false, true);
			}
			// else, continue (indexes are same, or neither has an index)
		}

		// Reached the end of at least one path, so either paths are same (end, end), or one path contains the other.
		return std::make_tuple(lhs_it, rhs_it, false, false);
	}

	std::tuple<
		bool,
		bool
	>
	qualified_path::indexed_after_divergence(qualified_path const& lhs, qualified_path const& rhs)
	{
		const_iterator lhs_it, rhs_it;
		bool lhs_idx, rhs_idx;
		std::tie(lhs_it, rhs_it, lhs_idx, rhs_idx) = divergence(lhs, rhs);

		bool lhs_result = lhs_idx || lhs.subpath(lhs_it).contains_indexes();
		bool rhs_result = rhs_idx || rhs.subpath(rhs_it).contains_indexes();
		return std::make_tuple(lhs_result, rhs_result);
	}

#if 0
	bool qualified_path::match_indexes(qualified_path const& lhs, qualified_path const& rhs)
	{
		auto div = divergence(lhs, rhs);
		return 


		/*
		basically, each section is of the form : >name[i] with the brackets optional.
		if name1 != name2 then return success
		else if no brackets or i1 == i2 then continue to next section
		else return failure.
		*/

		for(
			const_iterator lhs_it = std::begin(lhs), rhs_it = std::begin(rhs);
			lhs_it != std::end(lhs) && rhs_it != std::end(rhs);
			++lhs_it, ++rhs_it
			)
		{
			// Are both names identical?
			if(!component::same_name(*lhs_it, *rhs_it))
			{
				// No, diverged, so no need for further comparison
				return true;
			}
			else
			{
				// Yes

				// TODO: Suspect maybe this should just be a return false, not an error??????
				// (since a path to a repeat widget itself will not have an index)
				// Or, should xxx>repeater successfully match xxx>repeater[i]>xxx ????????
				assert(lhs_it->has_index() == rhs_it->has_index());

				// Do the components have indexes?
				if(lhs_it->has_index())
				{
					// Yes, are they the same?
					if(component::same_index(*lhs_it, *rhs_it))
					{
						// Yes, proceed
						continue;
					}
					else
					{
						// No, indexes differ, match failed
						return false;
					}
				}
				else
				{
					// No, so proceed
					continue;
				}
			}
		}

		return true;
	}
#endif

	qualified_path& qualified_path::operator+= (qualified_path const& rhs)
	{
		m_components.insert(
			end(),
			std::begin(rhs),
			std::end(rhs)
			);
		return *this;
	}

	qualified_path qualified_path::operator+ (qualified_path const& rhs) const
	{
		qualified_path result(*this);
		result += rhs;
		return result;
	}

	void qualified_path::pop()
	{
		m_components.erase(begin() + size() - 1);
	}

	bool qualified_path::operator== (qualified_path const& rhs) const
	{
		return m_components == rhs.m_components;
	}

	bool qualified_path::operator!= (qualified_path const& rhs) const
	{
		return !(*this == rhs);
	}

	bool qualified_path::operator< (qualified_path const& rhs) const
	{
		auto count = size();
		auto rhs_count = rhs.size();
		auto common_count = std::min(count, rhs_count);
		for(size_t i = 0; i < common_count; ++i)
		{
			auto const& comp = m_components[i];
			auto const& rhs_comp = rhs.m_components[i];
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

	qualified_path::component const& qualified_path::operator[] (int idx) const
	{
		return m_components[idx];
	}

	std::ostream& operator<< (std::ostream& os, qualified_path const& qp)
	{
		os << qp.to_string();
		return os;
	}
}



