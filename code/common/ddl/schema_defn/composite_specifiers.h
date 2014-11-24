// composite_specifiers.h

#ifndef __WB_DDL_COMPOSITE_SPECIFIERS_H
#define __WB_DDL_COMPOSITE_SPECIFIERS_H

#include "defn_node.h"
#include "composite_defn_node.h"
#include "../values/navigator.h"

#include <utility>
#include <vector>


namespace ddl {

	class define_children
	{
	public:
		define_children()
		{}

	public:
		define_children operator() (node_name name, defn_node defn)
		{
			auto result = *this;
			result.children_.push_back(std::make_pair(name, defn));
			return result;
		}

	public:
		std::vector< std::pair< node_name, defn_node > > children_;
	};


	class composite_specifier
	{
	public:
		composite_specifier(composite_defn_node&& node): node_(node)
		{}

	public:
		composite_specifier operator() (define_children const& children)
		{
			auto result = std::move(*this);
			for(auto const& c : children.children_)
			{
				result.node_.add(c.first, c.second);
			}
			return result;
		}

		operator defn_node()
		{
			return std::move(node_);
		}

	private:
		composite_defn_node node_;
	};

}


#endif

