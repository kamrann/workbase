
#include "manual_thrusters_test.h"

#include "thrusters/thruster_presets.h"

#include <Eigen/Dense>

#include <boost/tuple/tuple.hpp>


manual_thrusters_test_app::manual_thrusters_test_app(PolycodeView *view):
	thruster_sys(thruster_cfg),
	EventHandler()
{
	core = new POLYCODE_CORE(view, SCREEN_WIDTH, SCREEN_HEIGHT, false, false, 0, 0, 90);

	CoreServices::getInstance()->getResourceManager()->addArchive("Resources/default.pak");
	CoreServices::getInstance()->getResourceManager()->addDirResource("default", false);

	Screen *screen = new Screen();			

	ship_base = new ScreenShape(ScreenShape::SHAPE_RECT, SHIP_BASE_SIZE, SHIP_BASE_SIZE);
	ship_base->setColor(0.0,0.0,0.0,1.0);
	ship_base->colorAffectsChildren = false;
	ship_base->setPosition(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2);
	ship_base->setRotation(0);	// degrees
	screen->addChild(ship_base);

/*	ship_front = new ScreenShape(ScreenShape::SHAPE_CIRCLE, SHIP_BASE_SIZE, SHIP_BASE_SIZE, 3);
	ship_front->setColor(0.0,0.0,0.0,1.0);
	double temp = SHIP_BASE_SIZE - sqrt((1.7320508 * 0.5 * SHIP_BASE_SIZE * 1.7320508 * 0.5 * SHIP_BASE_SIZE) -
		(0.5 * 1.7320508 * 0.5 * SHIP_BASE_SIZE * 0.5 * 1.7320508 * 0.5 * SHIP_BASE_SIZE));
	ship_front->setPosition(0, -(SHIP_BASE_SIZE - temp));
	ship_front->setRotation(-90);	// degrees
	ship_base->addChild(ship_front);
*/
	thruster_cfg = thruster_presets::square_minimal();
	thruster_sys.sync_to_cfg();

	int const num_thrusters = thruster_cfg.thrusters.size();
	thruster_shapes.resize(num_thrusters);

	// TODO: For now rotations are hard coded
	thruster_shapes[0] = new ScreenShape(ScreenShape::SHAPE_CIRCLE, THRUSTER_SIZE, THRUSTER_SIZE, 3);
	thruster_shapes[0]->setRotation(0);

	thruster_shapes[1] = new ScreenShape(ScreenShape::SHAPE_CIRCLE, THRUSTER_SIZE, THRUSTER_SIZE, 3);
	thruster_shapes[1]->setRotation(180);

	thruster_shapes[2] = new ScreenShape(ScreenShape::SHAPE_CIRCLE, THRUSTER_SIZE, THRUSTER_SIZE, 3);
	thruster_shapes[2]->setRotation(180);

	thruster_shapes[3] = new ScreenShape(ScreenShape::SHAPE_CIRCLE, THRUSTER_SIZE, THRUSTER_SIZE, 3);
	thruster_shapes[3]->setRotation(0);


	for(int t = 0; t < num_thrusters; ++t)
	{
		thruster_shapes[t]->setPosition(Vector3(
			thruster_cfg[t].pos[0] * SHIP_BASE_SIZE / 2,
			thruster_cfg[t].pos[1] * SHIP_BASE_SIZE / 2,
			thruster_cfg[t].pos[2] * SHIP_BASE_SIZE / 2));
		ship_base->addChild(thruster_shapes[t]);
	}

//	thrust = 0;
	linearSpeed = Vector3(0, 0, 0);
	
//	torque = 0;
	rotationSpeed = 0;
	
	core->getInput()->addEventListener(this, InputEvent::EVENT_KEYDOWN);
	core->getInput()->addEventListener(this, InputEvent::EVENT_KEYUP);	
}

manual_thrusters_test_app::~manual_thrusters_test_app()
{

}

void manual_thrusters_test_app::handleEvent(Event *e)
{
	if(e->getDispatcher() == core->getInput())
	{
		InputEvent *inputEvent = (InputEvent*)e;
		
		switch(e->getEventCode())
		{
		case InputEvent::EVENT_KEYDOWN:
			switch (inputEvent->keyCode())
			{
			case KEY_0:
			case KEY_1:
			case KEY_2:
			case KEY_3:
			case KEY_4:
			case KEY_5:
			case KEY_6:
			case KEY_7:
			case KEY_8:
			case KEY_9:
				{
					size_t t_index = inputEvent->keyCode() - KEY_0;
					if(t_index < thruster_cfg.thrusters.size())
					{
						thruster_sys[t_index].t.engage();
					}
				}
				break;

			case KEY_SPACE:
				{
					linearSpeed = Vector3(0, 0, 0);
					rotationSpeed = 0;
				}
				break;
			}
			break;
			
		case InputEvent::EVENT_KEYUP:
			switch (inputEvent->key)
			{
			case KEY_0:
			case KEY_1:
			case KEY_2:
			case KEY_3:
			case KEY_4:
			case KEY_5:
			case KEY_6:
			case KEY_7:
			case KEY_8:
			case KEY_9:
				{
					size_t t_index = inputEvent->keyCode() - KEY_0;
					if(t_index < thruster_cfg.thrusters.size())
					{
						thruster_sys[t_index].t.disengage();
					}
				}
				break;
			}
			break;			
		}
	}
}

bool manual_thrusters_test_app::Update()
{
	Number elapsed = core->getElapsed();

	dim_traits::force_t thrust;
	dim_traits::torque_t torque;
	boost::tie(thrust, torque) = thruster_sys.calc_resultants();

	Number theta = ship_base->getRotation() * 2.0 * PI / 360.0;
	Vector2d thrust_world = Rotation2D< double >(theta) * Vector2d(thrust[0], thrust[1]);
	linearSpeed += to_polycode_vector(thrust_world) * elapsed * 50;
	rotationSpeed += torque[2] * elapsed * 10;

	ship_base->setPosition(ship_base->getPosition() + linearSpeed * elapsed);
	ship_base->setRotation(ship_base->getRotation() + elapsed * rotationSpeed);

	size_t const num_thrusters = thruster_cfg.thrusters.size();
	for(int t = 0; t < num_thrusters; ++t)
	{
		if(!thruster_sys[t].t.is_engaged())
		{
			thruster_sys[t].t.cool_down(elapsed);
		}

		thruster_shapes[t]->setColor(1.0, 1.0 - thruster_sys[t].t.temperature(), 1.0 - thruster_sys[t].t.temperature(), 1.0);
	}
	
    return core->updateAndRender();
}
