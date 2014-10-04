// wt_server.cpp

#include "wt_server.h"
#include "range.h"

#include "util/open_url.h"

#include <Wt/WApplication>
#include <Wt/WEnvironment>
#include <Wt/WServer>
#include <Wt/WContainerWidget>

#include <string>
#include <sstream>
#include <memory>
#include <map>


unsigned long next_id = 0;
std::map< unsigned long, display_data > display_data_mp;	// TODO: Prob needs synchronization?


class wt_display_app:
	public Wt::WApplication
{
public:
	wt_display_app(const Wt::WEnvironment& env): Wt::WApplication(env)
	{
		auto title = env.getParameterValues("name")[0];
		setTitle(title);

		m_display_id = std::stoul(env.getParameterValues("id")[0]);

		auto const& dd = display_data_mp.at(m_display_id);
		switch(dd.type)
		{
			case DisplayType::Range:
			root()->addWidget(new range_display(boost::any_cast<range_dd>(dd.data)));
			break;
		}
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
	the_server->addEntryPoint(Wt::Application, boost::bind(&create_wt_application, _1), "/nn_cmd");

	return the_server->start();
}

void terminate_wt_server()
{
	the_server->stop();
}


void create_wt_display(display_data data)
{
	auto const id = next_id++;
	display_data_mp[id] = data;

	std::stringstream url;
	url << "http://";
	url << "127.0.0.1:"; // TODO: Can't get WServer http address??
	url << Wt::WServer::instance()->httpPort();
	url << "/nn_cmd?name=foo";	// TODO:
	url << "&id=" << id;

	open_url(url.str(), SW_SHOWNA);
}



