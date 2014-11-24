// plot_query.cpp

#include "plot_query.h"

#include "evo_database/evo_db.h"

#include "params/param_tree.h"
#include "params/param_accessor.h"

#include <yaml-cpp/yaml.h>


namespace ga {

	bool compare_equal(
		prm::param_tree /*const&*/ pt1,
		prm::param_tree /*const&*/ pt2,
		prm::qualified_path const& ignore)
	{
		auto acc1 = prm::param_accessor{ &pt1 };
		auto acc2 = prm::param_accessor{ &pt2 };

		auto ig_node_1 = acc1.node_at(ignore);
		auto ig_node_2 = acc2.node_at(ignore);

		auto& tree1 = pt1.tree();
		auto& tree2 = pt2.tree();

		if(ig_node_1)
		{
			tree1.remove_branch(ig_node_1);
		}

		if(ig_node_2)
		{
			tree2.remove_branch(ig_node_2);
		}

		// TODO: implement comparison function for gtrees
		return YAML::Dump(pt1.convert_to_yaml()).compare(
			YAML::Dump(pt2.convert_to_yaml())) == 0;
	}

	/*
	Retrieves evo_run records matching the provided params, with the exception of that specified as the axis param.
	*/
	std::vector< dbo::ptr< evo_run > > plot_query(evodb_session& dbs, prm::param_tree const& match_params, prm::qualified_path const& axis_param)
	{
		dbo::Transaction t(dbs);
		dbo::collection< dbo::ptr< evo_run > > query = dbs.find< evo_run >();

		std::vector< dbo::ptr< evo_run > > result;
		for(auto ptr : query)
		{
			auto as_yaml = YAML::Load(ptr->params);
			prm::param_tree pt = prm::param_tree::generate_from_yaml(as_yaml);

			if(compare_equal(pt, match_params, axis_param))
			{
				result.push_back(ptr);
			}
		}

		t.commit();
		return result;
	}

}




