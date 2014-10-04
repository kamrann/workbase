// generic_tree_depthfirst.hpp

/*
Depth first traversal of a generic_tree.

Traversal operators:
TODO: May want to add to these signatures
PreOp: void (node_descriptor)
InOp: void (node_descriptor)
PostOp: void (node_descriptor)
*/

#ifndef __WB_GENERIC_TREE_DEPTH_FIRST_H
#define __WB_GENERIC_TREE_DEPTH_FIRST_H

#include "generic_tree.hpp"


namespace wb {
	namespace gtree {

		struct null_op
		{
			template <
				typename NodeDescriptor
			>
			inline void operator() (NodeDescriptor node) const
			{}
		};

		template <
			typename Tree,
			typename PreOp,
			typename InOp,
			typename PostOp
		>
		void depth_first_traversal
		(
		Tree const& tree,
		typename Tree::node_descriptor node,	// Only subtree rooted at this node is traversed
		PreOp const& pre_op = null_op{},
		InOp const& in_op = null_op{},
		PostOp const& post_op = null_op{}
		)
		{
			pre_op(node);
			auto children = tree.children(node);
			auto count = children.size();
			size_t index = 0;
			for(auto c : children)
			{
				depth_first_traversal(tree, c, pre_op, in_op, post_op);

				++index;
				if(index != count)
				{
					in_op(node);
				}
			}
			post_op(node);
		}

		template <
			typename Tree,
			typename PreOp,
			typename InOp,
			typename PostOp
		>
		void depth_first_traversal
		(
		Tree const& tree,
		PreOp const& pre_op = null_op{},
		InOp const& in_op = null_op{},
		PostOp const& post_op = null_op{}
		)
		{
			depth_first_traversal(tree, tree.get_root(), pre_op, in_op, post_op);
		}

	}
}


#endif


