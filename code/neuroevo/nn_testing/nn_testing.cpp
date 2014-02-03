
#include "nn_testing.h"

#include "thrusters/thruster_presets.h"
#include "polycode_util.h"

#include <Eigen/Dense>

#include <boost/tuple/tuple.hpp>

#include <chrono>
#include <sstream>
#include <string>


typedef angular_full_stop< WorldDimensionality::dim2D > problem_t;
const std::string nn_filename = "C:/Users/Cameron/Documents/GitHub/workbase/resource/neuroevo/nns/angular_full_stop_square_minimal.nn";


nn_testing_app::nn_testing_app(PolycodeView *view, PolycodeView *graph_view):
	ship_st(),//thruster_cfg),
	thruster_sys(nullptr),
	EventHandler()
{
	core = new POLYCODE_CORE(view, SCREEN_WIDTH, SCREEN_HEIGHT, false, false, 0, 0, 90);

//	graph_core = new POLYCODE_CORE(graph_view, 400, 300, false, false, 0, 0, 10);//90);

	CoreServices::getInstance()->getResourceManager()->addArchive("C:/Users/Cameron/Documents/GitHub/workbase/resource/common/polycode/default.pak");
	CoreServices::getInstance()->getResourceManager()->addDirResource("default", false);

	rgen.seed(static_cast< uint32_t >(std::chrono::high_resolution_clock::now().time_since_epoch().count() & 0xffffffff));

	ship_st.thruster_cfg = boost::shared_ptr< thruster_config< dim > >(new thruster_config< dim >(thruster_presets::square_minimal()));
	ship_st.ang_velocity = random_val< dim_traits::angular_velocity_t >(rgen);

	thruster_sys.cfg = ship_st.thruster_cfg;
	thruster_sys.sync_to_cfg();

	frame_by_frame = true;
	advance = false;

	Screen *screen = new Screen();

	scene = new ScreenEntity;
	scene->setScale(SHIP_BASE_SIZE, SHIP_BASE_SIZE);
	scene->setPosition(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2);
	screen->addChild(scene);

	ship_base = new ScreenShape(ScreenShape::SHAPE_RECT, 2.0, 2.0);//SHIP_BASE_SIZE, SHIP_BASE_SIZE);
	ship_base->setColor(0.0,0.0,0.0,1.0);
	ship_base->colorAffectsChildren = false;
	ship_base->setPosition(to_polycode_vector(ship_st.position));//SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2);
	ship_base->setRotation(ship_st.orientation * 360 / (2.0 * PI));	// degrees
	scene->addChild(ship_base);

//	thruster_cfg = ;
//	ship_st.thruster_sys.sync_to_cfg();

	nn.create_from_file(nn_filename.c_str());

	int const num_thrusters = ship_st.thruster_cfg->num_thrusters();
	thruster_shapes.resize(num_thrusters);

	// TODO: For now rotations are hard coded
	double T_SIZE = (double)THRUSTER_SIZE / SHIP_BASE_SIZE;
	thruster_shapes[0] = new ScreenShape(ScreenShape::SHAPE_CIRCLE, T_SIZE, T_SIZE, 3);
	thruster_shapes[0]->setRotation(0);

	thruster_shapes[1] = new ScreenShape(ScreenShape::SHAPE_CIRCLE, T_SIZE, T_SIZE, 3);
	thruster_shapes[1]->setRotation(180);

	thruster_shapes[2] = new ScreenShape(ScreenShape::SHAPE_CIRCLE, T_SIZE, T_SIZE, 3);
	thruster_shapes[2]->setRotation(180);

	thruster_shapes[3] = new ScreenShape(ScreenShape::SHAPE_CIRCLE, T_SIZE, T_SIZE, 3);
	thruster_shapes[3]->setRotation(0);


	for(int t = 0; t < num_thrusters; ++t)
	{
		thruster_shapes[t]->setPosition(to_polycode_vector((*ship_st.thruster_cfg)[t].pos));
		ship_base->addChild(thruster_shapes[t]);
	}


	//st = target_orientation::input_state_fn< boost::random::mt19937 >() (rgen);

	//facing = 0;
	ship_base->setRotation(ship_st.orientation * 360.0 / (2.0 * PI));

/*	lin_vel_line = new ScreenLine(Vector2(0, 0), Vector2(st.lin_vel[0] * 30, st.lin_vel[1] * 30));
	ship_base->addChild(lin_vel_line);
	lin_acc_line = nullptr;
*/
#if 0
	grid = new ScreenEntity;
	grid->setColor(0.5, 0.5, 0.5, 1.0);
	scene->addChild(grid);
	scene->moveChildBottom(grid);
	grid->setPosition(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2);

/*	for(int h = 0; h < 11; ++h)
	{
		horiz_bg[h] = new ScreenLine(
			Vector2(-5 * GRID_SCALE, (h - 5) * GRID_SCALE),
			Vector2(5 * GRID_SCALE, (h - 5) * GRID_SCALE)
			);
		grid->addChild(horiz_bg[h]);
	}
*/
	for(int v = 0; v < 11; ++v)
	{
		vert_bg[v] = new ScreenLine(
			Vector2((v - 5) * GRID_SCALE, -5 * GRID_SCALE),
			Vector2((v - 5) * GRID_SCALE, 5 * GRID_SCALE)
			);
		grid->addChild(vert_bg[v]);
	}
#endif

	lin_speed_lbl = new ScreenLabel("", 24);
	screen->addChild(lin_speed_lbl);

	ang_speed_lbl = new ScreenLabel("", 24);
	ang_speed_lbl->setPositionY(24);
	screen->addChild(ang_speed_lbl);

	core->getInput()->addEventListener(this, InputEvent::EVENT_KEYDOWN);
	core->getInput()->addEventListener(this, InputEvent::EVENT_KEYUP);	
}

nn_testing_app::~nn_testing_app()
{

}

void nn_testing_app::handleEvent(Event *e)
{
	if(e->getDispatcher() == core->getInput())
	{
		InputEvent *inputEvent = (InputEvent*)e;
		
		switch(e->getEventCode())
		{
		case InputEvent::EVENT_KEYDOWN:
			switch (inputEvent->keyCode())
			{
			case KEY_HOME:
				{
					scene->setPosition(Vector2(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2) - ship_base->getPosition2D());
				}
				break;

			case KEY_f:
				{
					frame_by_frame = !frame_by_frame;
					advance = false;
				}
				break;

			case KEY_SPACE:
				if(frame_by_frame)
				{
					advance = true;
				}
				break;

			case KEY_c:
				{

				}
				break;
			}
			break;
			
		case InputEvent::EVENT_KEYUP:
			switch (inputEvent->key)
			{
			}
			break;			
		}
	}
}

void nn_testing_app::UpdateDisplayText()
{
	std::stringstream ss;
	std::fixed(ss);
	ss.precision(2);

	dim_traits::force_t thrust;
	dim_traits::torque_t torque;
	//boost::tie(thrust, torque) = ship_st.thruster_cfg->calc_resultants( TODO:

	ss << "v = (" << ship_st.lin_velocity << ") - F = [" ;//<< thrust << "]";
	lin_speed_lbl->setText(ss.str().c_str());

	ss.str("");
	// TODO: omega = \u03C9, tau = \u03C4, think needs wstring, but then Eigen complains...
	ss << "w = (" << ship_st.ang_velocity << ") - T = [" ;//<< torque << "]";
	ang_speed_lbl->setText(ss.str().c_str());
}

bool nn_testing_app::Update()
{
	if(frame_by_frame)
	{
		if(!advance)
		{
			return core->Update();
		}
		advance = false;
	}

	Number elapsed = core->getElapsed();
	double timestep = 0.01;

	std::vector< double > inputs;
	problem_t::state problem_st;
	problem_st.agents.resize(1);
	problem_st.agents[0] = ship_st;

	problem_t::nn_agent_impl< 2, 0 > agent(ship_st.thruster_cfg->num_thrusters());
	//agent.nn = boost::shared_ptr< FANN::neural_net >(new FANN::neural_net(nn));
	agent.nn = nn;
	thruster_base::thruster_activation ta = agent(problem_st.get_sensor_state(0));
	
	dim_traits::force_t thrust;
	dim_traits::torque_t torque;
	boost::tie(thrust, torque) = ship_st.thruster_cfg->calc_resultants(ta, ship_st.c_of_mass);

	double thruster_power = 0.02;//0.001;

	dim_traits::force_t thrust_W = dim_traits::transform_dir(thrust, ship_st.orientation);
	ship_st.lin_velocity += thrust_W * thruster_power * timestep;

	dim_traits::torque_t torque_W = dim_traits::transform_dir(torque, ship_st.orientation);
	ship_st.ang_velocity += torque_W * thruster_power * timestep;
/* todo:
	st.delta -= st.omega * timestep;
	// Keep delta in the range [-Pi, Pi)
	st.delta = std::fmod(st.delta + boost::math::double_constants::pi, 2.0 * boost::math::double_constants::pi) -
		boost::math::double_constants::pi;
*/
	ship_st.position += ship_st.lin_velocity * timestep;
			
	// Now update the bot's world space facing, using the new angular velocity
	ship_st.orientation += ship_st.ang_velocity * timestep;
	// TODO: facing = std::fmod(facing, 2.0 * boost::math::double_constants::pi);

/*	Vector2d vel2d_world = Eigen::Rotation2D< double >(facing) * Vector2d(st.lin_vel[0], st.lin_vel[1]);
*/

	for(size_t i = 0; i < ship_st.thruster_cfg->num_thrusters(); ++i)
	{
		if(ta[i])
		{
			thruster_sys[i].t.engage();
		}
		else
		{
			thruster_sys[i].t.disengage();
		}
	}
	
	ship_base->setPosition(to_polycode_vector(ship_st.position));// * SHIP_BASE_SIZE / 2);

#if 0
	// Instead of moving the ship, move the background
	Vector3 cur_pos = grid->getPosition();
	cur_pos.x += -SCREEN_WIDTH / 2 + vel2d_world[0] * SHIP_BASE_SIZE / 2;
//	cur_pos.y += -SCREEN_HEIGHT / 2 + vel2d_world[1] * SHIP_BASE_SIZE / 2;
	cur_pos.x = std::fmod(cur_pos.x, GRID_SCALE);
//	cur_pos.y = std::fmod(cur_pos.y, GRID_SCALE);

	grid->setPosition(SCREEN_WIDTH / 2 + cur_pos.x, /*SCREEN_HEIGHT / 2 +*/ cur_pos.y);
#endif

	ship_base->setRotation(ship_st.orientation * 360.0 / (2.0 * PI));

/*	if(lin_vel_line != nullptr)
	{
		ship_base->removeChild(lin_vel_line);
		delete lin_vel_line;
	}
	lin_vel_line = new ScreenLine(Vector2(0, 0), Vector2(st.lin_vel[0] * 30, st.lin_vel[1] * 30));
	ship_base->addChild(lin_vel_line);

	if(lin_acc_line != nullptr)
	{
		ship_base->removeChild(lin_acc_line);
		delete lin_acc_line;
	}
	lin_acc_line = new ScreenLine(Vector2(0, 0), Vector2(thrust[0] * 30, thrust[1] * 30));
	lin_acc_line->setColor(1.0, 0.0, 0.0, 1.0);
	ship_base->addChild(lin_acc_line);
*/

	for(int t = 0; t < ship_st.thruster_cfg->num_thrusters(); ++t)
	{
		if(!thruster_sys[t].t.is_engaged())
		{
			thruster_sys[t].t.cool_down(elapsed);
		}

		thruster_shapes[t]->setColor(1.0, 1.0 - thruster_sys[t].t.temperature(), 1.0 - thruster_sys[t].t.temperature(), 1.0);
	}

	UpdateDisplayText();
	
	return core->updateAndRender();
}
