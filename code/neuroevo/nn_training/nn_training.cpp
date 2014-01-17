/*
Fast Artificial Neural Network Library (fann)
Copyright (C) 2003-2012 Steffen Nissen (sn@leenissen.dk)

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#define NOMINMAX

#include "sim.h"

//#include "problems/simple_force_torque.h"
//#include "problems/full_stop.h"
#include "problems/angular_full_stop.h"
//#include "problems/linear_full_stop.h"
//#include "problems/target_orientation.h"

#include <windows.h>

#include <iostream>


using namespace std;


int main()
{
	cout.precision(4);
	std::fixed(cout);

	cout << ("Initialising...") << endl;

	WorldDimensionality const dim = WorldDimensionality::dim2D;
	typedef simulation< angular_full_stop< dim > > sim_t;
	sim_t sim;
	sim.init();

	cout << ("Training...") << endl;


	int const NumEpochs = 200;

	sim_t::processed_obj_val_t highest_ov(0.0);
	//boost::shared_ptr< FANN::neural_net >
	FANN::neural_net overall_best;
	for(int e = 0; e < NumEpochs; ++e)
	{
		sim.run_epoch();

		//boost::shared_ptr< FANN::neural_net >
		FANN::neural_net best;
		sim_t::processed_obj_val_t obj_value = sim.get_fittest(best);

		if(/*
		   !fitness.unfit && highest_fitness.unfit || fitness.unfit == highest_fitness.unfit &&
		   */
		   obj_value > highest_ov || true)	// TODO: true for Pareto
		{
			highest_ov = obj_value;
			overall_best = best;
		}

//		cout << ("Epoch ") << (e + 1) << (": Average = ") << sim.ga.average_fitness << (", Best = ") << sim.ga.best_fitness << endl;
	}

	cout << "Training completed. Best fitness: " << highest_ov << std::endl;

	overall_best.save("test.nn");

/*	cout << ("Testing network.\n");

	test_data = fann_read_train_from_file("../datasets/robot.test");

	fann_reset_MSE(ann);
	for(i = 0; i < fann_length_train_data(test_data); i++)
	{
		fann_test(ann, test_data->input[i], test_data->output[i]);
	}
	printf("MSE error on test data: %f\n", fann_get_MSE(ann));

	printf("Saving network.\n");

	fann_save(ann, "robot_float.net");

	printf("Cleaning up.\n");
	fann_destroy_train(train_data);
	fann_destroy_train(test_data);
	fann_destroy(ann);
*/
	return 0;
}
