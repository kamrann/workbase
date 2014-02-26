// thruster_presets.h

#ifndef __THRUSTER_PRESETS_H
#define __THRUSTER_PRESETS_H

#include "thruster.h"


class thruster_presets
{
public:
	// 4 thrusters at corners of [-1, 1] square, minimal setup for complete control
	static thruster_config< WorldDimensionality::dim2D > square_minimal()
	{
		thruster_config< WorldDimensionality::dim2D > cfg;
		cfg.thrusters.resize(4);
		cfg.thrusters[0].pos = Vector2d(-1.0, 1.0);
		cfg.thrusters[0].dir = Vector2d(1.0, 0.0);
		cfg.thrusters[1].pos = Vector2d(1.0, -1.0);
		cfg.thrusters[1].dir = Vector2d(-1.0, 0.0);
		cfg.thrusters[2].pos = Vector2d(1.0, 1.0);
		cfg.thrusters[2].dir = Vector2d(-1.0, 0.0);
		cfg.thrusters[3].pos = Vector2d(-1.0, -1.0);
		cfg.thrusters[3].dir = Vector2d(1.0, 0.0);
		return cfg;
	}

	// 8 thrusters at corners of [-1, 1] square, allowing for linear thrust along 2 axis
	static thruster_config< WorldDimensionality::dim2D > square_complete()
	{
		thruster_config< WorldDimensionality::dim2D > cfg;
		cfg.thrusters.resize(8);
		cfg.thrusters[0].pos = Vector2d(-1.0, 1.0);
		cfg.thrusters[0].dir = Vector2d(1.0, 0.0);
		cfg.thrusters[1].pos = Vector2d(1.0, -1.0);
		cfg.thrusters[1].dir = Vector2d(-1.0, 0.0);
		cfg.thrusters[2].pos = Vector2d(1.0, 1.0);
		cfg.thrusters[2].dir = Vector2d(-1.0, 0.0);
		cfg.thrusters[3].pos = Vector2d(-1.0, -1.0);
		cfg.thrusters[3].dir = Vector2d(1.0, 0.0);
		cfg.thrusters[4].pos = Vector2d(-1.0, 1.0);
		cfg.thrusters[4].dir = Vector2d(0.0, -1.0);
		cfg.thrusters[5].pos = Vector2d(1.0, -1.0);
		cfg.thrusters[5].dir = Vector2d(0.0, 1.0);
		cfg.thrusters[6].pos = Vector2d(1.0, 1.0);
		cfg.thrusters[6].dir = Vector2d(0.0, -1.0);
		cfg.thrusters[7].pos = Vector2d(-1.0, -1.0);
		cfg.thrusters[7].dir = Vector2d(0.0, 1.0);
		return cfg;
	}
};


#endif


