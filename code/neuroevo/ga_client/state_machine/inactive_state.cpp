// inactive_state.cpp

#include "inactive_state.h"
#include "control_fsm.h"
#include "../plot_query.h"

#include "evo_database/evo_db.h"

#include "params/param_tree.h"
#include "params/qualified_path.h"

#include "wt_cmdline_server/wt_server.h"
#include "wt_displays/chart.h"


namespace ga_control {
	namespace fsm {

		void inactive::on_dbplot(clsm::ev_cmd< dbplot_cmd > const& cmd)
		{
			auto& ctx = context< ga_controller >();
			prm::param_accessor acc{ &ctx.ptree };

			auto path = acc.find_path("std_dev");
//				prm::qualified_path{ "root" };
//			path += std::string{ "ga_cfg" };
//			path += std::string{ "pop_size" };
			auto q = ga::plot_query(*ctx.db_session, ctx.ptree, path);

			auto const& axis_param_data = acc.find_param(path);
			prm::ParamType xval_type = axis_param_data.type;

			display_data dd;
			dd.type = chart_display_defn::ID;
			chart_dd chart_data;
			chart_data.x_axis_name = axis_param_data.name;
			chart_data.series.push_back(chart_dd::series_data{ "max fitness", {} });
			int x = 0;

//			std::map< 

			for(auto const& data_pt : q)
			{
				// TODO: duplicating from plot_query()...
				auto as_yaml = YAML::Load(data_pt->params);
				prm::param_tree ptree = prm::param_tree::generate_from_yaml(as_yaml);
				//
				boost::any xval;
				switch(xval_type)
				{
					case prm::ParamType::Integer:
					xval = prm::extract_as< int >(prm::param_accessor{ &ptree }.find_value(path));
					break;
					case prm::ParamType::RealNumber:
					xval = prm::extract_as< double >(prm::param_accessor{ &ptree }.find_value(path));
					break;

					// TODO: Category chart for bool/enum/etc
					default:
					xval = x;
					break;
				}

				chart_data.x_vals.push_back(xval);
				// TODO: Should be quicker to access, perhaps store highest in evo_run table
				// TODO: currently assuming real obj val
				double highest = -std::numeric_limits< double >::max();
				Wt::Dbo::Transaction t(*ctx.db_session);
				for(auto const& gen : data_pt->generations)
				{
					auto gen_highest = YAML::Load(gen->highest_obj_val).as< double >();
					if(gen_highest > highest)
					{
						highest = gen_highest;
					}
				}
				t.commit();
				chart_data.series[0].y_vals.push_back(highest);

				++x;
			}
			dd.data = chart_data;
			auto id = create_wt_display(dd);
			// TODO: Temp hack
			std::this_thread::sleep_for(std::chrono::seconds(5));
			//
			update_display(id);
		}

	}
}





