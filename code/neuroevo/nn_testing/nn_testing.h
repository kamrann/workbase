
#include "thrusters/ship_state.h"
#include "problems/full_stop.h"
#include "problems/angular_full_stop.h"
#include "problems/linear_full_stop.h"
#include "problems/target_orientation.h"

#include <Eigen/Dense>

#include <Polycode.h>
#include "PolycodeView.h"
#include "Polycode2DPhysics.h"

#include "doublefann.h"
#include "fann_cpp.h"

#include <boost/random/mersenne_twister.hpp>

#include <array>


using namespace Polycode;


class nn_testing_app: public EventHandler
{
public:
	static const WorldDimensionality dim = WorldDimensionality::dim2D;
	typedef DimensionalityTraits< dim > dim_traits;

public:
	nn_testing_app(PolycodeView *view, PolycodeView *graph_view);
    ~nn_testing_app();

	void handleEvent(Event *e);
	void UpdateDisplayText();
    bool Update();
    
private:
	Core *core, *graph_core;
	ScreenShape *ship_base;
	std::vector< ScreenShape* > thruster_shapes;

	ScreenLine* lin_vel_line;
	ScreenLine* lin_acc_line;

	ScreenEntity* grid;
	std::array< ScreenLine*, 11 > horiz_bg;
	std::array< ScreenLine*, 11 > vert_bg;

	ScreenLabel *lin_speed_lbl, *ang_speed_lbl;

	ScreenEntity *scene;

	bool frame_by_frame;
	bool advance;
//	bool display_local;

	enum {
		SCREEN_WIDTH = 800,
		SCREEN_HEIGHT = 600,

		GRID_SCALE = 30,

		SHIP_BASE_SIZE = 40,
		THRUSTER_SIZE = 8,
	};

	//thruster_config< dim > thruster_cfg;
	thruster_system< dim > thruster_sys;

	boost::random::mt19937 rgen;

	FANN::neural_net nn;
	
	//target_orientation::state st;
	ship_state< dim > ship_st;

public:
	EIGEN_MAKE_ALIGNED_OPERATOR_NEW
};
