// collective_obj_val_ftr.h

#ifndef __COLLECTIVE_OBJ_VAL_FTR_H
#define __COLLECTIVE_OBJ_VAL_FTR_H


/*
This ftr is applied once to the entire population, after individual objective functions have been evaluated.
It allows modification of objective values (including if necessary their types) based on the obj vals of the whole population.
*/

/*
Default functor is the identity - raw obj value is assigned directly to output
*/
template <
	typename RawObjValueType		// Any type
>
struct coll_obj_val_identity
{
	typedef RawObjValueType			raw_obj_val_t;
	typedef raw_obj_val_t			processed_obj_val_t;

	template < typename ProcessedObjValueAccessor >
	struct RawAccessorDefn
	{
		// Define the raw accessor type to be the same as the processed accessor type
		typedef ProcessedObjValueAccessor	processed_obj_val_acc_t;
		
		struct raw_accessor
		{
			processed_obj_val_acc_t& processed_acc;

			raw_accessor(processed_obj_val_acc_t& _processed_acc, size_t _pop_size): processed_acc(_processed_acc)
			{}

			inline raw_obj_val_t& operator[] (int i)
			{
				return processed_acc[i];
			}
		};

		typedef raw_accessor raw_obj_val_acc_t;
	};

	template < typename ProcessedObjValueAccessor >
	inline void operator() (typename RawAccessorDefn< ProcessedObjValueAccessor >::raw_obj_val_acc_t const& raw, size_t const count, ProcessedObjValueAccessor& out) const
	{
		/* Do nothing ???
		for(size_t i = 0; i < count; ++i)
		{
			out[i] = raw[i];
		}
		*/
	}
};


/*
This version replaces a pareto obj val with an integer pareto ranking value.
*/
template <
	typename RawObjValueType		// Must be pareto type
>
struct coll_obj_val_pareto_ranking
{
	typedef RawObjValueType		raw_obj_val_t;
	typedef double				processed_obj_val_t;

	template < typename ProcessedObjValueAccessor >
	struct RawAccessorDefn
	{
		// Define the raw accessor type to be the same as the processed accessor type
		typedef ProcessedObjValueAccessor	processed_obj_val_acc_t;

		struct raw_accessor: public std::vector< raw_obj_val_t >
		{
			raw_accessor(processed_obj_val_acc_t& _processed_acc, size_t _pop_size):
				std::vector< raw_obj_val_t >(_pop_size)
			{}
		};

		typedef raw_accessor	raw_obj_val_acc_t;
	};

	template < typename ProcessedObjValueAccessor >
	inline void operator() (typename RawAccessorDefn< ProcessedObjValueAccessor >::raw_obj_val_acc_t const& raw, size_t const count, ProcessedObjValueAccessor& out) const
	{
		std::vector< processed_obj_val_t > ranks(count);
		raw_obj_val_t::calc_dominance_depths(raw.begin(), raw.end(), ranks.begin());
		for(size_t i = 0; i < count; ++i)
		{
			// Negate the rank since processed objective values are currently treated as bigger is better
			out[i] = -ranks[i];
		}
	}
};


#endif


