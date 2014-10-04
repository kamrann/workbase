// param_tree.h

#ifndef __WB_PARAM_TREE_H
#define __WB_PARAM_TREE_H

#include "param.h"
#include "param_fwd.h"
#include "schema.h"

#include "util/gtree/generic_tree.hpp"

#include <boost/optional.hpp>


namespace YAML {
	class Node;
}

namespace prm
{
	class param_tree
	{
	public:
		static param_tree generate_from_schema(schema::schema_node s, param_accessor acc);
		static param_tree generate_from_schema(schema::schema_node s);
		
		static param_tree generate_from_yaml(YAML::Node const& node);
		YAML::Node convert_to_yaml() const;

		std::string rootname() const;

	public:
		struct param_data
		{
			ParamType type;
			std::string name;
			boost::optional< param > value;
		};

	private:
		struct edge_data
		{
			boost::optional< int > repeat_idx;
		};

		typedef wb::gtree::generic_tree< param_data, edge_data > tree_t;

		struct YAMLParamNode
		{
			static const int Name = 0;
			static const int Type = 1;
			static const int Value = 2;
		};

	private:
		static param generate_default_terminal(schema::schema_node s);
		void generate_from_schema(schema::schema_node s, tree_t::node_descriptor node, param_accessor acc);
		static param generate_terminal_from_yaml(YAML::Node yaml, ParamType type);
		void generate_from_yaml(YAML::Node yaml, tree_t::node_descriptor node);

		inline tree_t& tree()
		{
			return m_tree;
		}

	private:
		tree_t m_tree;

		friend class param_accessor;
	};
}


#endif


