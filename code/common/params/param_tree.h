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
		// TODO: Suspect having adding schema provider, should get rid of schema node parameter
		static param_tree generate_from_schema(schema::schema_node s, param_accessor acc, schema::schema_provider_map_handle sch_mp);
		static param_tree generate_from_schema(schema::schema_node s, schema::schema_provider_map_handle sch_mp);
		
		static param_tree generate_from_yaml(YAML::Node const& node);
		YAML::Node convert_to_yaml() const;

		std::string rootname() const;

		void dump(std::ostream& os) const;

	public:
		struct repeat_extra
		{
			std::string contents_name;
/*			size_t next_instance_num;

			repeat_extra():
				next_instance_num{ 0 }
			{}
			*/
		};

		struct param_data
		{
			ParamType type;
			std::string name;
			boost::optional< param > value;
			boost::any extra;
		};

	public:
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

	public:
		bool add_repeat_instance(tree_t::node_descriptor rpt_node, param_accessor acc, schema::schema_provider_map_handle sch_mp);
		bool remove_repeat_instance(tree_t::node_descriptor rpt_node, unsigned int index);
		qualified_path node_qpath(tree_t::node_descriptor node) const;

		YAML::Node convert_to_yaml(tree_t::node_descriptor node) const;
		void generate_from_yaml(YAML::Node yaml, tree_t::node_descriptor node);

		inline tree_t& tree()
		{
			return m_tree;
		}

	private:
		static param generate_default_terminal(schema::schema_node s);
		void generate_from_schema(schema::schema_node s, tree_t::node_descriptor node, param_accessor acc, schema::schema_provider_map_handle sch_mp);
		static param generate_terminal_from_yaml(YAML::Node yaml, ParamType type);

	private:
		tree_t m_tree;

		friend class param_accessor;
	};
}


#endif


