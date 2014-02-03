/*
* Copyright (C) 2008 Emweb bvba, Heverlee, Belgium.
*
* See the LICENSE file for terms of use.
*/

#include "../nn_training/sim.h"

#include "problems/angular_full_stop.h"

#include <doublefann.h>

#include <Wt/WServer>
#include <Wt/WApplication>
#include <Wt/WBreak>
#include <Wt/WContainerWidget>
#include <Wt/WLineEdit>
#include <Wt/WPushButton>
#include <Wt/WText>
#include <Wt/WTextArea>

#include <boost/shared_ptr.hpp>
#include <boost/thread.hpp>

// c++0x only, for std::bind
// #include <functional>

using namespace Wt;

/*
* A simple hello world application class which demonstrates how to react
* to events, read input, and give feed-back.
*/
class HelloApplication : public WApplication
{
public:
	HelloApplication(const WEnvironment& env);

private:
	void run_alg();

public:
	void generation_cb(std::string txt);
	void completion_cb(std::string txt);

public:
	//WText *output_;
	WTextArea *output_;

	boost::shared_ptr< boost::thread > ga_thread;
};

/*
* The env argument contains information about the new session, and
* the initial request. It must be passed to the WApplication
* constructor so it is typically also an argument for your custom
* application constructor.
*/
HelloApplication::HelloApplication(const WEnvironment& env)
: WApplication(env)
{
	setTitle("GA Test");                               // application title

	WPushButton *button = new WPushButton("Run", root());              // create a button
	button->setFocus();
	//button->setMargin(5, Left);                            // add 5 pixels margin

	root()->addWidget(new WBreak());                       // insert a line break

	output_ = new WTextArea(root());                         // empty text
	output_->setReadOnly(true);

	/*
	* Connect signals with slots
	*
	* - simple Wt-way
	*/
	button->clicked().connect(this, &HelloApplication::run_alg);
}

void HelloApplication::generation_cb(std::string txt)
{
//	HelloApplication* inst = (HelloApplication*)WApplication::instance();

	output_->setText(txt);
	triggerUpdate();
}

void HelloApplication::completion_cb(std::string txt)
{
//	HelloApplication* inst = (HelloApplication*)WApplication::instance();

	output_->setText(txt);
	triggerUpdate();
	enableUpdates(false);

	ga_thread->join();
	ga_thread.reset();
}

void run_fn(WApplication* app)
	//std::string session_id)
{
	std::stringstream out;
	out.precision(4);
	std::fixed(out);

	out << ("Initialising...") << std::endl;

	WorldDimensionality const dim = WorldDimensionality::dim2D;
	typedef simulation< angular_full_stop< dim > > sim_t;
	sim_t sim;
	sim.init();

	out << ("Training...") << std::endl;

	int const NumEpochs = 10;

	sim_t::processed_obj_val_t highest_ov(0.0);
	//boost::shared_ptr< FANN::neural_net >
	FANN::neural_net overall_best;
	for(int e = 0; e < NumEpochs; ++e)
	{
		sim.run_epoch(out);

		//boost::shared_ptr< FANN::neural_net >
		FANN::neural_net best;
		sim_t::processed_obj_val_t obj_value = sim.get_fittest(best);

		if (/*
			!fitness.unfit && highest_fitness.unfit || fitness.unfit == highest_fitness.unfit &&
			*/
			obj_value > highest_ov || true)	// TODO: true for Pareto
		{
			highest_ov = obj_value;
			overall_best = best;
		}

		//		cout << ("Epoch ") << (e + 1) << (": Average = ") << sim.ga.average_fitness << (", Best = ") << sim.ga.best_fitness << endl;

		std::string session_id = app->sessionId();	// TODO: Need lock to make this call???
		WServer::instance()->post(session_id, boost::bind(&HelloApplication::generation_cb, (HelloApplication*)app, out.str()));
	}

	out << "Training completed. Best fitness: " << highest_ov << std::endl;

	std::string session_id = app->sessionId();	// TODO: Need lock to make this call???
	WServer::instance()->post(session_id, boost::bind(&HelloApplication::completion_cb, (HelloApplication*)app, out.str()));
}

void HelloApplication::run_alg()
{
	enableUpdates(true);	// TODO: Right place? And where to disable?
	ga_thread.swap(boost::shared_ptr< boost::thread >(new boost::thread(run_fn, this)));//sessionId())));
}

WApplication *createApplication(const WEnvironment& env)
{
	/*
	* You could read information from the environment to decide whether
	* the user has permission to start a new application
	*/
	return new HelloApplication(env);
}

int main(int argc, char **argv)
{
	/*
	* Your main method may set up some shared resources, but should then
	* start the server application (FastCGI or httpd) that starts listening
	* for requests, and handles all of the application life cycles.
	*
	* The last argument to WRun specifies the function that will instantiate
	* new application objects. That function is executed when a new user surfs
	* to the Wt application, and after the library has negotiated browser
	* support. The function should return a newly instantiated application
	* object.
	*/
	return WRun(argc, argv, &createApplication);
}
