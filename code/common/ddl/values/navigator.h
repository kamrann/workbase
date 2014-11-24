// navigator.h

#ifndef __WB_DDL_NAVIGATOR_H
#define __WB_DDL_NAVIGATOR_H

#include "value_node.h"
#include "../path.h"

#include <stack>


namespace ddl {

	class navigator
	{
	public:
		navigator();
		navigator(value_node nd);

	private:
		navigator(value_node nd, std::stack< value_node > anc, path pth);

	public:
		bool is_parent() const;
		navigator parent() const;
		bool at_leaf() const;
		path where() const;

		// For when located at composite
		std::vector< node_name > child_names() const;
		bool is_child(node_name child_id) const;
		navigator child(node_name child_id) const;
		navigator operator[] (node_name child_id) const;
		// TODO: Convenience overload. Note that this is making assumption that node_name is typedefed as a string
		navigator operator[] (char const* const child_id) const;
		
		// For when located at list
		size_t list_num_items() const;
		navigator operator[] (size_t item_idx) const;

		navigator deactivated() const;
		bool is_active() const;

		value_node get() const;
//		ref_resolver get_ref_resolver() const;

		navigator operator[] (path const& pth) const;

		operator bool() const;

		struct find_results
		{
			struct result_location
			{
				size_t up_count;	// number of steps up ancestors, before...
				size_t down_count;	// number of steps down

				result_location(
					size_t up = 0,
					size_t down = 0
					):
					up_count{ up },
					down_count{ down }
				{}

				inline bool operator< (result_location const& rhs) const
				{
					return up_count < rhs.up_count ||
						up_count == rhs.up_count && down_count < rhs.down_count;
				}
			};

			typedef std::vector< path > path_list;

			std::map< result_location, path_list > by_location;

			find_results operator| (find_results const&) const;
			find_results& operator|= (find_results const&);
		};

		find_results::path_list find_descendent_r_id(node_id id) const;
		find_results find_by_id(node_id id) const;

//		bool match()
		find_results::path_list find_child(node_name str/*, find_results::result_location offset = {}*/) const;
		find_results::path_list find_descendent_r(node_name str/*, find_results::result_location offset = {}*/) const;
		find_results find_ancestor(node_name str/*, find_results::result_location offset = {}*/) const;
		find_results find(node_name str) const;

		find_results find_descendent(node_name str) const;

	private:
		value_node node_;
		std::stack< value_node > ancestor_stk_;
		path path_;
		bool deactivated_;
	};

}


#endif


