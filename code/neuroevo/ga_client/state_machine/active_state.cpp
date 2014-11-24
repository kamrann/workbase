// active_state.cpp

#include "active_state.h"
#include "control_fsm.h"

// temp
#include "../ga_params.h"
//

#include "genetic_algorithm/fitness_assignment.h"

#include "wt_cmdline_server/wt_server.h"
#include "wt_displays/chart.h"

#include "params/param_accessor.h"


namespace ga_control {
	namespace fsm {

		active::active(my_context ctx):
			base(ctx)
		{
			reg_cmd< exit_cmd >([]()->qi::rule< clsm::iter_t, exit_cmd(), clsm::skip_t >{ return qi::lit("exit")[phx::nothing]; });
			reg_cmd< reset_cmd >(wrap_grammar< reset_cmd_parser< clsm::iter_t > >());

			auto& ga_ctx = context< ga_controller >();
			auto acc = prm::param_accessor{ &ga_ctx.ptree };

			auto& defn = ga_ctx.ga_def;
			domain = defn.generate_domain(acc);
			alg = defn.generate_alg(acc, domain);

			//ga_rseed = static_cast<uint32_t>(std::chrono::high_resolution_clock::now().time_since_epoch().count() & 0xffffffff);
			reset_ga(boost::none);

			ga_ctx.serializer->serialize_start(ga_ctx.ptree.convert_to_yaml());
		}

		active::~active()
		{
			context< ga_controller >().serializer->serialize_end();
		}

		void active::on_reset(clsm::ev_cmd< reset_cmd > const& cmd)
		{
			boost::optional< unsigned int > rseed;
			switch(cmd.seed.method)
			{
				case reset_cmd::seed_data::SeedMethod::Random:
				rseed = static_cast<uint32_t>(std::chrono::high_resolution_clock::now().time_since_epoch().count() & 0xffffffff);
				break;

				case reset_cmd::seed_data::SeedMethod::Fixed:
				rseed = cmd.seed.value;
				break;

				default:	// Same - leave rseed uninitialized so argument will be ignored and seed left unchanged
				break;
			}

			if(rseed)
			{
				context< ga_controller >().set_seed(*rseed);
			}
			//reset_ga(rseed);
		}

		void active::on_chart(clsm::ev_cmd< chart_cmd > const& cmd)
		{
			/*				sys::state_value_id_list ids;
			for(auto const& v : cmd.values)
			{
			ids.push_back(sys::state_value_id::from_string(v));
			}
			*/
			add_chart();// ids);
		}

		sc::result active::react(clsm::ev_cmd< reset_cmd > const& cmd)
		{
			on_reset(cmd);
			return transit< active >();
		}

		// Initialize the system
		void active::reset_ga(boost::optional< unsigned int > rseed)
		{
/*			if(rseed)
			{
				ga_rseed = *rseed;
			}
*/
			{
				std::lock_guard< std::mutex > lock_{ ga_mx };

				alg->reset(context< ga_controller >().ga_rseed);

/*				rgen.seed(context< ga_controller >().ga_rseed);

				// Initialize first generation
				for(auto& idv : pop)
				{
					idv.gn = domain->create_random_genome(rgen);
				}
*/			}
//			generation = 0;

			clear_all_chart_data();

/*			for(auto& drawer : drawers)
			{
				update_display(drawer.first);
			}
*/		}

		// Advances the system by a single update frame
		bool active::do_generation(frame_update_cfg const& cfg)
		{
			{
				std::lock_guard< std::mutex > lock_{ ga_mx };

				alg->epoch();
			}

/*			if(cfg.output_updates)	todo: probably want to reenable this, so can configure to output best fitness, diversity, etc
			{
				output_state_values(update_vals);
			}
*/
			if(cfg.store_chart_series_data)
			{
				store_chart_series_data();
			}

			if(cfg.redraw_charts)
			{
				redraw_all_charts();
			}

			auto diversity = domain->population_genetic_diversity(alg->population());
			context< ga_controller >().serializer->serialize_generation(alg->generation(), alg->population(), diversity);

			//++generation;

			return true;
		}

		bool active::do_generations(size_t count, frame_update_cfg const& cfg)
		{
			for(size_t i = 0; i < count; ++i)
			{
				if(!do_generation(cfg))
				{
					return false;
				}
			}
			return true;
		}

/*		void active::output_state_values(sys::state_value_id_list const& svids)
		{
			std::lock_guard< std::mutex > lock_{ sys_mx };

			for(auto v_id : svids)
			{
				auto bound = sys->get_state_value_binding(v_id);
				auto value = sys->get_state_value(bound);
				std::stringstream ss;
				ss << v_id.to_string() << ": " << value;
				context< system_controller >().output_sink(ss.str());
			}
		}
*/
		void active::add_chart()//sys::state_value_id_list vals)
		{
			display_data dd;
			dd.type = chart_display_defn::ID;
			chart_dd chart_data;
			chart_defn defn;
			{
				std::lock_guard< std::mutex > lock_{ ga_mx };

				chart_data.series.push_back(chart_dd::series_data{ "max fitness", {} });
				chart_data.series.push_back(chart_dd::series_data{ "genetic diversity", {} });

/*				for(auto const& val : vals)
				{
					chart_data.series.push_back(chart_dd::series_data{ val.to_string(), {} });
					defn.series.push_back(chart_defn::series_defn{ sys->get_state_value_binding(val) });
				}
*/			}
			dd.data = chart_data;
			auto id = create_wt_display(dd);
			charts[id] = std::move(defn);
		}

/*		void active::add_drawer()
		{
			display_data dd;
			dd.type = DisplayType::Drawer;
			drawer_dd drawer_data;
			drawer_data.mx = &sys_mx;
			{
				std::lock_guard< std::mutex > lock_{ sys_mx };
				drawer_data.drawer.reset(sys->get_drawer().release());	//m_sys_defn->get_drawer().release();
			}
			drawer_defn defn;
			dd.data = drawer_data;
			auto id = create_wt_display(dd);
			drawers[id] = std::move(defn);
		}
*/
		void active::clear_chart_data(unsigned long id)
		{
			auto& dd = get_display_data(id);
			auto& chart_data = boost::any_cast<chart_dd&>(dd.data);
			chart_data.x_vals.clear();
			for(auto& series : chart_data.series)
			{
				series.y_vals.clear();
			}
			chart_data.reload = true;
			release_display_data();

			update_display(id);
		}

		void active::clear_all_chart_data()
		{
			for(auto& ch : charts)
			{
				clear_chart_data(ch.first);
			}
		}

		void active::store_chart_series_data()
		{
			std::lock_guard< std::mutex > lock_{ ga_mx };

			for(auto const& entry : charts)
			{
				auto id = entry.first;
				auto& dd = get_display_data(id);
				auto& chart_data = boost::any_cast<chart_dd&>(dd.data);
				chart_data.x_vals.push_back(alg->generation());	// TODO: Support using state value on x axis (eg time)
				chart_data.series[0].y_vals.push_back(alg->population().max_fitness());
				chart_data.series[1].y_vals.push_back(domain->population_genetic_diversity(alg->population()));
				/*				for(size_t s_idx = 0; s_idx < entry.second.series.size(); ++s_idx)
				{
					auto const& series = entry.second.series[s_idx];
					auto val = sys->get_state_value(series.sv_bound_id);
					chart_data.series[s_idx].y_vals.push_back(val);
				}
*/				release_display_data();
			}
		}

		void active::redraw_all_charts()
		{
			for(auto const& entry : charts)
			{
				auto id = entry.first;
				update_display(id);
			}
		}

	}
}




