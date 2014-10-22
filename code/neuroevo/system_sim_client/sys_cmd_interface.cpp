// sys_cmd_interface.cpp

#include "sys_cmd_interface.h"
#include "cmd_parser.h"
//#include "sys_controller.h"
#include "control_fsm.h"

#include "system_sim/system.h"

#include "params/param_accessor.h"


namespace sys_control {

	class cmd_visitor: public boost::static_visitor < cmd_interface::cmd_fn >
	{
	public:
		cmd_visitor(//controller& _ctrl):
			fsm::system_controller& _ctrl):
			ctrl(_ctrl)
		{}

		inline result_type operator() (reset_cmd const& cmd)
		{
			auto& _ctrl = ctrl;
			return [&_ctrl, cmd]
			{
				auto ev = fsm::ev_reset{};
				ev.seed = cmd.seed;
				_ctrl.process_event(ev);
			};
		}

		inline result_type operator() (step_cmd const& cmd)
		{
			auto& _ctrl = ctrl;
			return [&_ctrl, cmd]
			{
				auto ev = fsm::ev_step{};
				ev.frames = cmd.frames;
				_ctrl.process_event(ev);
			};
		}

		inline result_type operator() (run_cmd const& cmd)
		{
			auto& _ctrl = ctrl;
			return [&_ctrl, cmd]
			{
				auto ev = fsm::ev_run{};
				ev.frame_rate = cmd.frame_rate;
				_ctrl.process_event(ev);
			};
		}

		inline result_type operator() (stop_cmd const& cmd)
		{
			auto& _ctrl = ctrl;
			return [&_ctrl, cmd]
			{
				_ctrl.process_event(fsm::ev_pause{});
			};
		}

		inline result_type operator() (get_cmd const& cmd)
		{
			auto& _ctrl = ctrl;
			return [&_ctrl, cmd]
			{
				auto val_ids = sys::state_value_id_list{};
				for(auto const& str : cmd.values)
				{
					val_ids.push_back(sys::state_value_id::from_string(str));
				}
				_ctrl.process_event(fsm::ev_get{ val_ids });
			};
		}

		inline result_type operator() (chart_cmd const& cmd)
		{
			auto& _ctrl = ctrl;
			return [&_ctrl, cmd]
			{
#if 0
				if(_ctrl.mode() != Mode::Stopped)
				{
					return;
				}

				auto val_ids = sys::state_value_id_list{};
				for(auto const& str : cmd.values)
				{
					val_ids.push_back(sys::state_value_id::from_string(str));
				}

				_ctrl.add_chart(std::move(val_ids));
#endif
			};
		}

		inline result_type operator() (drawer_cmd const& cmd)
		{
			auto& _ctrl = ctrl;
			return [&_ctrl, cmd]
			{
#if 0
				if(_ctrl.mode() != Mode::Stopped)
				{
					return;
				}

				_ctrl.add_drawer();
#endif
			};
		}

		//controller& ctrl;
		fsm::system_controller& ctrl;
	};


	sys_cmd_interface::sys_cmd_interface(
		//controller& ctrl,
		fsm::system_controller& ctrl,
		std::ostream& out):
		cmd_interface{ out },
		m_ctrl{ ctrl }
	{

	}

	std::string sys_cmd_interface::prompt() const
	{
		return m_prompt;
	}

	sys_cmd_interface::preprocessed_cmd sys_cmd_interface::preprocess_cmd(std::string cmd_str)
	{
		sys_control::cmd_parser< std::string::const_iterator > parser;

		auto it = std::begin(cmd_str);
		command cmd;
		auto result = qi::phrase_parse(it, std::end(cmd_str), parser, qi::space_type{}, cmd);

		if(!result)
		{
			return sys_cmd_interface::preprocessed_cmd{ std::string{ "parse failure" } };
		}

		cmd_visitor vis{ m_ctrl };
		return boost::apply_visitor(vis, cmd);
	}

	void sys_cmd_interface::dispatch(cmd_fn fn)
	{
		m_prompt = m_ctrl.post(fn);
	}

}


