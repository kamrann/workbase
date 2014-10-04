// generic_system_coordinator.h

#ifndef __GENERIC_SYSTEM_COORDINATOR_H
#define __GENERIC_SYSTEM_COORDINATOR_H

#include "rtp_interface/systems/rtp_system.h"
#include "rtp_interface/systems/rtp_system_agents_data.h"
#include "rtp_interface/systems/rtp_controller.h"
#include "rtp_interface/systems/rtp_interactive_controller.h"

#include "system_coordinator.h"
#include "wt_system_widgets/system_display_widget.h"
#include "wt_system_widgets/properties_widget.h"

#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

#include <chrono>
#include <thread>
#include <mutex>


class generic_sys_coordinator: public system_coordinator
{
public:
	typedef boost::shared_ptr< rtp::i_controller >		controller_ptr_t;

	enum {
		RenderPeriod = 1,		// Per update (1 = Render every update, 2 = Render every second update, etc)
		// TODO: ChartPeriod = 1,		// As above
	};

	typedef sys_display_widget						widget_t;
//	typedef properties_chart_widget					chart_widget_t;

public:
	generic_sys_coordinator(
		std::unique_ptr< rtp::i_system_factory > sys_factory,
		rtp::agents_data agents);
	~generic_sys_coordinator();

	virtual std::pair< Wt::WWidget*, Wt::WWidget* > initialize() override;
	virtual void cancel() override;
	virtual void restart() override;

	virtual void register_system_finish_callback(std::function< void() > cb) override;

	void update_widgets();// bool force_update);

private:
	void terminate_system();
	void time_check();

	void on_widget_interaction(rtp::interactive_input const& input);
	void on_animation_update();
	void on_system_update();

private:
	std::unique_ptr< rtp::i_system_factory > m_sys_factory;
	rtp::agents_data m_agents_spec;
	std::unique_ptr< rtp::i_system > m_sys;

	widget_t* m_widget;
	i_properties_widget* m_props_widget;
	unsigned int m_step;
	
	enum class SystemState {
		Active,
		Completed,
	};

	SystemState m_system_st;

	boost::asio::io_service m_io;
	std::thread m_update_thread;
	boost::asio::deadline_timer m_timer;
	std::mutex m_sys_mutex;
	Wt::WApplication* m_app;
	bool m_request_termination;
	bool m_ready_to_display;
	std::function< void() > m_finish_cb;

	//
	std::chrono::high_resolution_clock::time_point m_last_update_tm;
	float m_avg_update_period;
	//

	std::map< rtp::agent_id_t, controller_ptr_t > m_controllers;
};


#endif

