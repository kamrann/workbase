// direct_fitness.h

#ifndef __DIRECT_FITNESS_H
#define __DIRECT_FITNESS_H


/*
Fitness value ::= Objective value
*/
template <
	typename ProcessedObjValueType
>
struct direct_fitness
{
	typedef ProcessedObjValueType	processed_obj_value_t;
	typedef processed_obj_value_t	fitness_t;

	template < typename FitnessAccessor >
	struct ProcessedAccessorDefn
	{
		typedef FitnessAccessor		fitness_acc_t;

		// Provide indexable access to the objective value of individuals
		// In this case, they are simply the fitness values embedded in the individual type
		struct obj_val_accessor
		{
			fitness_acc_t& fitnesses;

			obj_val_accessor(fitness_acc_t& _f, size_t const _pop_size): fitnesses(_f)
			{}

			inline processed_obj_value_t& operator[] (int idx)
			{
				return fitnesses[idx];
			}
		};

		typedef obj_val_accessor	processed_obj_val_acc_t;
	};

	template < typename FitnessAccessor >
	inline void operator() (typename ProcessedAccessorDefn< FitnessAccessor >::processed_obj_val_acc_t& obj_vals, FitnessAccessor& fitnesses)) const
	{
		// Nothing to do, objective values have already been assigned in-place
	}

	template < typename FitnessAccessor >
	inline void operator() (typename ProcessedAccessorDefn< FitnessAccessor >::processed_obj_val_acc_t& obj_vals, FitnessAccessor& fitnesses)) const
	{
		pop size????
	}
};


#endif


