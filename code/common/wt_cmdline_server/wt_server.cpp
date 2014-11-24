// wt_server.cpp

#include "wt_server.h"

#include "terminal.h"

#include "commands/cmd_input.h"

#include "util/open_url.h"

#include "wt_custom_widgets/WCommandLine.h"
#include "wt_custom_widgets/WOutputConsoleBase.h"

#include <Wt/WApplication>
#include <Wt/WEnvironment>
#include <Wt/WServer>
#include <Wt/WContainerWidget>
#include <Wt/WVBoxLayout>

#include <string>
#include <sstream>
#include <memory>
#include <map>
#include <deque>
#include <mutex>
#include <condition_variable>


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

std::map< std::string, std::unique_ptr< display_defn > > disp_type_mp;

std::map< unsigned long, registered_display > display_data_mp;	// TODO: Prob needs synchronization?
std::mutex dd_mx;


class wt_terminal_impl:
	public i_terminal
{
public:
	wt_terminal_impl(std::string session_id, WCommandLine* cmdline, WOutputConsoleBase* console):
		m_session_id{ session_id },
		m_cmdline{ cmdline },
		m_console{ console }
	{
		m_cmd_input.set_input_callback([this](wb::cmd::command c)
		{
			m_cmdline->set_command(c);
		});

		m_cmdline->on_command().connect(std::bind([this](std::string cmd_str){ m_cmd_input.dispatch(cmd_str); }, std::placeholders::_1));
		
		m_cmdline->on_key().connect(std::bind([this](Wt::Key wt_key){
			wb::cmd::key key;
			// TODO: Wt key binding map
			switch(wt_key)
			{
				case Wt::Key_Up:
				key.code = wb::cmd::key::KeyCode::Up;
				break;
				case Wt::Key_Down:
				key.code = wb::cmd::key::KeyCode::Down;
				break;
				default:
				return;
			}
			m_cmd_input.process_key(key);
		}, std::placeholders::_1));
//			std::bind(handler, std::placeholders::_1));

		//m_cmd_input.set_command_handler
	}

public:
	virtual void register_command_handler(cmd_handler_fn handler) override
	{
		post([this, handler]
		{
			//m_cmdline->on_command().connect(std::bind(handler, std::placeholders::_1));
			m_cmd_input.set_command_handler(handler);
		});
	}
	
	virtual void output(std::string text) override
	{
		{
			std::lock_guard< std::mutex > lock_{ m_q_mx };

			m_q.push_back(text);
		}

		post([this]
		{
			{
				std::lock_guard< std::mutex > lock_{ m_q_mx };

				while(!m_q.empty())
				{
					// TODO: inefficient
					m_console->add_element(m_q.front());
					m_q.pop_front();
				}
			}

			Wt::WApplication::instance()->triggerUpdate();
		});
	}

	virtual void set_prompt(std::string prompt) override
	{
		post([this, prompt] { m_cmdline->set_prompt(prompt); Wt::WApplication::instance()->triggerUpdate(); });
	}

private:
	void post(std::function< void() > fn)
	{
		Wt::WServer::instance()->post(m_session_id, fn);
	}

private:
	std::string m_session_id;
	WCommandLine* m_cmdline;
	WOutputConsoleBase* m_console;

	std::mutex m_q_mx;
	std::deque< std::string > m_q;

	// TODO: This should be at a non-Wt specific level
	wb::cmd::command_input m_cmd_input;
};

std::mutex term_mx;
std::condition_variable term_cv;
std::unique_ptr< wt_terminal_impl > terminal;


class wt_display_app:
	public Wt::WApplication
{
public:
	wt_display_app(const Wt::WEnvironment& env): Wt::WApplication(env)
	{
//		setCssTheme("polished");

		auto title = env.getParameterValues("name")[0];
		setTitle(title);

		auto type = env.getParameterValues("type")[0];
		if(type == "terminal")
		{
			auto layout_ = new Wt::WVBoxLayout{};
			layout_->setContentsMargins(0, 0, 0, 0);
			root()->setLayout(layout_);

			auto term = new terminal_display{};
			layout_->addWidget(term);

			{
				std::lock_guard< std::mutex > lock_{ term_mx };

				if(!terminal)
				{
					terminal = std::make_unique< wt_terminal_impl >(
						sessionId(),
						term->get_cmdline(),
						term->get_console()
						);
				}
				else
				{
					throw std::runtime_error("currently can't handler multiple sessions");
					// hack to allow page reload to just update to the new terminal
/*					*terminal = wt_terminal_impl{
						sessionId(),
						term->get_cmdline(),
						term->get_console()
					};
*/				}
			}
			term_cv.notify_one();
		}
		else if(type == "display")
		{
			m_display_id = std::stoul(env.getParameterValues("id")[0]);

			{
				std::lock_guard< std::mutex > lock_{ dd_mx };
				auto& rd = display_data_mp.at(m_display_id);
				rd.session_id = sessionId();

				Wt::WWidget* disp = nullptr;

				auto it = disp_type_mp.find(rd.dd.type);
				if(it == disp_type_mp.end())
				{
					throw std::runtime_error("invalid display type");
				}

				rd.disp = it->second->create(rd.dd.data);
				root()->addWidget(rd.disp->get_wt_widget());

				/*	call update or not?
				if(!rd.dd.data.empty())
				{
				rd.disp->update_from_data(rd.dd);
				}
				*/
			}
		}
		else
		{
			// TODO: What is effect of throwing here? Kill Wt server?
			throw std::runtime_error("invalid display class");
		}

		useStyleSheet("style/terminal.css");

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


i_terminal* create_wt_terminal(std::string name)
{
	std::stringstream url;
	url << "http://";
	url << "127.0.0.1:"; // TODO: Can't get WServer http address??
	url << Wt::WServer::instance()->httpPort();
	url << "/?name=" << name;//system%20simulation%20terminal";	// TODO:
	url << "&type=terminal";

	open_url(url.str(), SW_SHOWNA);

	std::unique_lock< std::mutex > lock_{ term_mx };
	term_cv.wait(lock_, []{ return (bool)terminal; });

	return terminal.get();
}


bool register_display_defn(std::unique_ptr< display_defn > defn)
{
	auto disp_type_name = defn->name();
	auto it = disp_type_mp.find(disp_type_name);
	if(it != disp_type_mp.end())
	{
		return false;
	}

	disp_type_mp[disp_type_name] = std::move(defn);
	return true;
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
	url << "&type=display";
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
			entry.disp->update_from_data(entry.dd.data);
		}
		Wt::WApplication::instance()->triggerUpdate();
	});
}



