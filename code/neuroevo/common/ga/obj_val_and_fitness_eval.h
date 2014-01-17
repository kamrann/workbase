// obj_val_and_fitness_eval.h

#ifndef __OBJ_VAL_AND_FITNESS_EVAL_H
#define __OBJ_VAL_AND_FITNESS_EVAL_H


/*
ObjectiveValueFunction
	-> RawObjectiveValueType
	CollectiveObjectiveValueFunction
		-> ProcessedObjectiveValueType
		FitnessAssignmentFunction
			-> FitnessType
			-> CollectiveObjectiveValuesAccessorType
		-> RawObjectiveValuesAccessorType
*/

/*
Provided by the problem domain:
	Trial data type
	Genome type

Need to specify:
	Resultant objective function (this includes component objective functions)
	Objective value collective processing function
	Fitness assignment function

Remainder is deduced automatically.
*/

typedef ...												trial_data_t;
typedef avg_speed_obj_fn								obj_val_fn_t;
typedef single_objective< trial_data_t, obj_val_fn_t >	resultant_obj_fn_t;
typedef resultant_obj_fn_t::obj_value_t					raw_obj_val_t;
typedef resultant_obj_fn_t::ofdata_t					ofdata_t;
typedef coll_obj_val_identity< raw_obj_val_t >			collective_obj_val_fn_t;
typedef collective_obj_val_fn_t::processed_obj_val_t	processed_obj_val_t;
typedef ranking_fitness< processed_obj_val_t >			fitness_assignment_fn_t;
typedef fitness_assignment_fn_t::fitness_t				fitness_t;
typedef individual< genome_t, fitness_t >				individual_t;
typedef std::vector< individual_t >						population_t;

typedef fitness_assignment_fn_t::ProcessedAccessorDefn< population_t >::processed_obj_val_acc_t		processed_obj_val_acc_t;
typedef collective_obj_val_fn_t::RawAccessorDefn< processed_obj_val_acc_t >::raw_obj_val_acc_t		raw_obj_val_acc_t;

{
	// Initialise trial data and objective function data
	trial_data_t trial_data[pop_size]: initialise
	ofdata_t ofdata[pop_size]

	// Run the trial
	for(trial timesteps)
	{
		for(i: population)
		{
			ofdata[i].update()
		}
	}

	// Evaluate the objective function for each individual based on data gathered in trial
	raw_obj_val_acc_t raw_obj_val_acc;
	resultant_obj_fn_t obj_fn;
	for(i: population)
	{
		ofdata[i].finalise()

		raw_obj_val_acc[i] = obj_fn.evaluate(ofdata[i], trial_data[i]);
	}

	processed_obj_val_acc_t processed_obj_val_acc(population);
	
	// Invoke the collective objective value processing
	collective_obj_val_fn_t() (raw_obj_val_acc, population.size(), processed_obj_val_acc);

	// Finally assign population fitness based on processed values
	fitness_assignment_fn_t() (processed_obj_val_acc, population);
}


#endif


