#include <Polycode.h>
#include "PolycodeView.h"
#include "Polycode2DPhysics.h"

#include "util/dimensionality.h"
#include "thrusters/thruster.h"


using namespace Polycode;


class manual_thrusters_test_app: public EventHandler {
public:
    manual_thrusters_test_app(PolycodeView *view);
    ~manual_thrusters_test_app();

	void handleEvent(Event *e);
    bool Update();
    
private:
	Core *core;
	ScreenShape *ship_base;
	std::vector< ScreenShape* > thruster_shapes;

	enum {
		SCREEN_WIDTH = 800,
		SCREEN_HEIGHT = 600,

		SHIP_BASE_SIZE = 40,
		THRUSTER_SIZE = 8,
	};

	static const WorldDimensionality dim = WorldDimensionality::dim2D;
	typedef DimensionalityTraits< dim > dim_traits;

	thruster_config< dim > thruster_cfg;
	thruster_system< dim > thruster_sys;

//	Number thrust;
	dim_traits::velocity_t linearSpeed;

//	Number torque;
	dim_traits::angular_velocity_t rotationSpeed;
};
