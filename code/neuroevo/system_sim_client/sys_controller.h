// sys_controller.h

#ifndef __WB_SYSSIM_CONTROLLER_H
#define __WB_SYSSIM_CONTROLLER_H

#include "system_sim/system_defn.h"
#include "system_sim/system.h"
#include "system_sim/system_state_values.h"

#include <boost/asio.hpp>

#include <string>
#include <memory>
#include <vector>
#include <mutex>


namespace asio = boost::asio;

namespace sys_control {

	enum class Mode {
//		Ready,
		Stopped,
		Running,
		Completed,
	};

	enum class CommandType {
		Step,
		Run,
		Stop,
		Get,
	};


	class controller
	{
	public:
		controller(
			std::shared_ptr< sys::i_system_defn > sys_defn,
			sys::system_ptr sys,
			sys::state_value_id_list update_vals,
			sys::state_value_id_list result_vals,
			std::ostream& out);
		void start();
		void post(std::function< void() > fn);
		void terminate();

	public:
		Mode mode()
		{
			return m_mode;
		}

	private:
		struct frame_update_cfg
		{
			bool output_updates;
			bool store_chart_series_data;
			bool redraw_charts;
			bool redraw_system;

			frame_update_cfg():
				output_updates{ true },
				store_chart_series_data{ true },
				redraw_charts{ true },
				redraw_system{ true }
			{}
		};

		bool update_frame(frame_update_cfg const& cfg);
		void output_values(sys::state_value_id_list const& vals);
		void output_update_values();
		void store_chart_series_data();
		void redraw_all_charts();
		void redraw_all_system_views();

		void add_chart(sys::state_value_id_list vals);
		void add_drawer();
//		void set_frame_rate(boost::optional< double > frame_rate);
		void reset_system(boost::optional< unsigned int > rseed);
		void run(boost::optional< double > frame_rate);
		void stop();
		void step(unsigned int frames);
		bool commands_waiting();
//		bool interrupted();
//		void interrupt();
		void do_run();
		void notify_completion();
		void clear_chart_data(unsigned long id);
		void clear_all_chart_data();

	private:
		asio::io_service m_io_service;
		std::unique_ptr< asio::io_service::work > m_io_work;
		std::ostream& m_os;

		std::shared_ptr< sys::i_system_defn > m_sys_defn;
		sys::system_ptr m_system;
		unsigned int m_sys_rseed;
		std::mutex m_sys_mx;

		size_t m_frame;
		sys::state_value_id_list m_update_vals;
		sys::state_value_id_list m_result_vals;
		Mode m_mode;
		boost::optional< double > m_run_framerate;

//		std::atomic< bool > m_interrupt;
//		std::thread m_run_thread;

		size_t m_cmds_waiting;
		std::mutex m_cmd_mx;

		struct chart_defn
		{
			struct series_defn
			{
				sys::state_value_bound_id sv_bound_id;
			};

			std::vector< series_defn > series;
		};

		std::map< unsigned long, chart_defn > m_charts;

		struct drawer_defn
		{};

		std::map< unsigned long, drawer_defn > m_drawers;

		friend class cmd_visitor;
		friend class sys_cmd_interface;
	};

}


#endif


