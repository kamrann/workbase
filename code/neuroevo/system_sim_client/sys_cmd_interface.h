// sys_cmd_interface.h

#ifndef __WB_SYS_CMD_INTERFACE_H
#define __WB_SYS_CMD_INTERFACE_H

#include "cmd_interface.h"
#include "params/param_fwd.h"


namespace sys_control {

	//class controller;
	namespace fsm {
		struct system_controller;
	}

	class sys_cmd_interface:
		public cmd_interface
	{
	public:
		sys_cmd_interface(
			//controller& ctrl,
			fsm::system_controller& ctrl,
			std::ostream& out);

	private:
		virtual std::string prompt() const override;
		virtual preprocessed_cmd preprocess_cmd(std::string cmd_str) override;

		virtual void dispatch(cmd_fn fn) override;

	private:
		//controller& m_ctrl;
		fsm::system_controller& m_ctrl;
		std::string m_prompt;
	};

}



#endif


