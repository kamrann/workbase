// wt_server.cpp

#include "wt_server.h"
#include "chart.h"
#include "drawer.h"

#include "util/open_url.h"

#include <Wt/WApplication>
#include <Wt/WEnvironment>
#include <Wt/WServer>
#include <Wt/WContainerWidget>

#include <string>
#include <sstream>
#include <memory>
#include <map>
#include <mutex>


unsigned long next_id = 0;

struct registered_display
{
	std::string session_id;
	display_data dd;
	display_base* disp;

	registered_display():
		disp{ nullptr }
	{}
};

std::map< unsigned long, registered_display > display_data_mp;	// TODO: Prob needs synchronization?
std::mutex dd_mx;


class wt_display_app:
	public Wt::WApplication
{
public:
	wt_display_app(const Wt::WEnvironment& env): Wt::WApplication(env)
	{
		auto title = env.getParameterValues("name")[0];
		setTitle(title);

		m_display_id = std::stoul(env.getParameterValues("id")[0]);

		{
			std::lock_guard< std::mutex > lock_{ dd_mx };
			auto& rd = display_data_mp.at(m_display_id);
			rd.session_id = sessionId();

			Wt::WWidget* disp = nullptr;
			switch(rd.dd.type)
			{
				case DisplayType::Chart:
				{
					auto const& data = boost::any_cast<chart_dd const&>(rd.dd.data);
					auto chart_disp = new chart_display(data);
					rd.disp = chart_disp;
					disp = chart_disp;
				}
				break;

				case DisplayType::Drawer:
				{
					auto const& data = boost::any_cast<drawer_dd const&>(rd.dd.data);
					auto drawer_disp = new drawer_display(data);
					rd.disp = drawer_disp;
					disp = drawer_disp;
				}
				break;
			}

			root()->addWidget(disp);

/*	call update or not?
if(!rd.dd.data.empty())
			{
				rd.disp->update_from_data(rd.dd);
			}
*/		}

		enableUpdates(true);
	}

private:
	unsigned long m_display_id;
};


Wt::WApplication* create_wt_application(const Wt::WEnvironment& env)
{
	return new wt_display_app(env);
}


std::unique_ptr< Wt::WServer > the_server;

bool start_wt_server(arg_list args)
{
	auto const num_args = args.size();
	char** c_args = new char*[num_args];
	size_t index = 0;
	for(auto a: args)
	{
		auto const buf_size = a.length() + 1;
		c_args[index] = new char[buf_size];
		strcpy_s(c_args[index], buf_size, a.c_str());
		++index;
	}

	the_server = std::make_unique< Wt::WServer >(args.front(), "./wt_config.xml");
	the_server->setServerConfiguration(num_args, c_args, WTHTTP_CONFIGURATION);
	the_server->addEntryPoint(Wt::Application, boost::bind(&create_wt_application, _1), "/");

	return the_server->start();
}

void terminate_wt_server()
{
	the_server->stop();
}


unsigned long create_wt_display(display_data data)
{
	auto const id = next_id++;

	{
		std::lock_guard< std::mutex > lock_{ dd_mx };
		display_data_mp[id].dd = data;
	}

	std::stringstream url;
	url << "http://";
	url << "127.0.0.1:"; // TODO: Can't get WServer http address??
	url << Wt::WServer::instance()->httpPort();
	url << "/?name=foo";	// TODO:
	url << "&id=" << id;

	open_url(url.str(), SW_SHOWNA);

	return id;
}

display_data& get_display_data(unsigned long id)
{
	dd_mx.lock();
	return display_data_mp.at(id).dd;
}

void release_display_data()
{
	dd_mx.unlock();
}

void update_display(unsigned long id)
{
	std::lock_guard< std::mutex > lock_{ dd_mx };
	auto const& entry = display_data_mp.at(id);
	Wt::WServer::instance()->post(entry.session_id, [id]
	{
		{
			std::lock_guard< std::mutex > lock_{ dd_mx };
			auto const& entry = display_data_mp.at(id);
			entry.disp->update_from_data(entry.dd);
		}
		Wt::WApplication::instance()->triggerUpdate();
	});
}



