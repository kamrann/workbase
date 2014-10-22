// active_state.cpp

#include "active_state.h"


namespace sys_control {
	namespace fsm {

		active::active(my_context ctx):
			my_base(ctx)
		{
			auto& sc_ctx = context< system_controller >();

			// Entering active system state, construct a system from the param tree
			auto acc = prm::param_accessor{ &sc_ctx.ptree };
			
			auto sys_type = prm::extract_as< prm::enum_param_val >(acc["sys_type"])[0];
			auto const& sys_defn = sc_ctx.sys_defns.at(sys_type);
			sys = sys_defn->create_system(acc);

			// And reset it
			sys_rseed = static_cast<uint32_t>(std::chrono::high_resolution_clock::now().time_since_epoch().count() & 0xffffffff);
			reset_system(boost::none);

			// Also store update state value ids
			auto update_value_names = prm::extract_as< prm::enum_param_val >(acc["updates"]);
			for(auto const& v : update_value_names)
			{
				update_vals.push_back(sys::state_value_id::from_string(v));
			}

			// And result state value ids
			auto result_value_names = prm::extract_as< prm::enum_param_val >(acc["results"]);
			for(auto const& v : result_value_names)
			{
				result_vals.push_back(sys::state_value_id::from_string(v));
			}
		}

		// Initialize the system
		void active::reset_system(boost::optional< unsigned int > rseed)
		{
			if(rseed)
			{
				sys_rseed = *rseed;
			}

			sys->set_random_seed(sys_rseed);
			sys->initialize();
			frame = 0;
			complete = false;
/*
			clear_all_chart_data();

			for(auto& drawer : m_drawers)
			{
				update_display(drawer.first);
			}
*/		}

		// Advances the system by a single update frame
		bool active::step_system(frame_update_cfg const& cfg)
		{
			auto still_going = sys->update(nullptr);

			if(cfg.output_updates)
			{
				output_state_values(update_vals);
			}

			++frame;

			return still_going;
		}

		bool active::step_system(size_t frames, frame_update_cfg const& cfg)
		{
			for(size_t i = 0; i < frames; ++i)
			{
				if(!step_system(cfg))
				{
					return false;
				}
			}
			return true;
		}

		void active::output_state_values(sys::state_value_id_list const& svids) const
		{
			// TODO: std::lock_guard< std::mutex > lock_{ m_sys_mx };

			for(auto v_id : svids)
			{
				auto bound = sys->get_state_value_binding(v_id);
				auto value = sys->get_state_value(bound);
				std::stringstream ss;
				ss << v_id.to_string() << ": " << value;
				context< system_controller >().output_sink(ss.str());
			}
		}

		sc::result active::react(ev_reset const& ev)
		{
			boost::optional< unsigned int > rseed;
			switch(ev.seed.method)
			{
				case reset_cmd::seed_data::SeedMethod::Random:
				rseed = static_cast<uint32_t>(std::chrono::high_resolution_clock::now().time_since_epoch().count() & 0xffffffff);
				break;

				case reset_cmd::seed_data::SeedMethod::Fixed:
				rseed = ev.seed.value;
				break;

				default:	// Same - leave rseed uninitialized so argument will be ignored and seed left unchanged
				break;
			}
			reset_system(rseed);

			return transit< not_running >();
		}

	}
}




