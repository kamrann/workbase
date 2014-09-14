// activation_functions.h

#ifndef __WB_NN_ACTIVATION_FUNCTIONS_CONCEPT_H
#define __WB_NN_ACTIVATION_FUNCTIONS_CONCEPT_H

#include <sstream>
#include <limits>


namespace nnet {

	/*
	TODO: Forget the enum.

	May need mpl typelists, or may be able to do using only variadic templates..?

	template <
		typename... Elements,	// Equivalent of the enum elements
		typename... Actions,	// Tag types representing properties which all the elements possess
	>
	class typed_enum
	{
		typedef unsigned int runtime_type;

		// This should provide value for each type based on its 0-based position in the type list.
		template < typename Element >
		struct runtime_value;
		//...

		template<
			typename Action,
			typename... Args
		>
		auto dispatch(runtime_type rt_value, Args... args) -> typename Action::return_type
		{
			switch(rt_value)
			{
			// somehow recursively unroll this switch for every runtime_value< Elem >::value in Elements
			case ?: return [Associated Element Type]::do_action< Action, Args... >(args...);
			}
		}
	};

	template<>
	class typed_enum< my_element_list, my_action_list >
	{
		template<
			typename Element,
			typename Action,
			typename... Args
		>
		auto dispatch_action() -> typename Action::return_type;

		template<
			typename Element
		>
		auto dispatch_action< Element, is_something >() -> is_something::return_type
		{
			return Element::is_something();
		}

		template<
			typename Element
		>
		auto dispatch_action< Element, get_something_else >(double x) -> get_something_else::return_type
		{
			return Element::get_something_else(x);
		}
	};

	Usage:

	class elemA
	{
		bool is_something()
		{
			return false;
		}

		double get_something_else(double x)
		{
			return x * 2;
		}
	};

	class elemB
	{
		// This way, without named action functions, makes defining the element classes a little more awkward,
		// but means there's no need to specialise the typed_enum template as above.
		template < typename Action, typename... Args >
		auto do_action(Args... args) -> typename Action::return_type;

		template <>
		auto do_action< is_something >() -> bool
		{
			return false;
		}

		template <>
		auto do_action< get_something_else >(double x) -> double
		{
			return x * 2;
		}
	};

	class Action1
	{
		typedef double return_type;
	};

	class Action2
	{
		typedef int return_type;
	};


	typedef typed_enum<
		list of element classes,
		list of action tags
	>
	my_enum;


	auto runtime_val = ...;
	auto result = my_enum::do< is_something >(runtime_val);

	or

	auto result = my_enum::is_something(runtime_val); ???

	*/

#if 0
	// Version 1

	enum class Fn {
		FnA,
		FnB,

		Runtime,
	};

	enum class Actions {
		Apply,
		IsSymmetric,
		IsBounded,
		IsBinary,
		GetRange,
	};

	template <
		Fn FnType = Fn::Runtime
	>
	class ActivationFunc;


	template <>
	class ActivationFunc < Fn::FnA >
	{
	public:
		template <
			Actions action,
			typename... Args
		>
		struct do_dispatch;

		template <>
		struct do_dispatch < Actions::Apply, double >
		{
			static inline auto invoke(double x) -> double
			{
				return x * 2;
			}
		};
	};

	template <>
	class ActivationFunc < Fn::FnB >
	{
	public:
		template <
			Actions action,
			typename... Args
		>
		struct do_dispatch;

		template <>
		struct do_dispatch < Actions::Apply, double >
		{
			static inline auto invoke(double x) -> double
			{
				return x * 3;
			}
		};
	};


	template <>
	class ActivationFunc< Fn::Runtime >
	{
	public:
		template <
			Actions action
		>
		struct return_type;

		template <>
		struct return_type < Actions::Apply >
		{
			typedef double type;
		};

		template <
			Actions action,
			typename... Args
		>
		static inline auto dispatch(Fn fn, Args... args) -> typename return_type< action >::type
		{
			switch(fn)
			{
				case Fn::FnA:
				return ActivationFunc< Fn::FnA >::template do_dispatch< action, Args... >::invoke(args...);
				case Fn::FnB:
				return ActivationFunc< Fn::FnB >::template do_dispatch< action, Args... >::invoke(args...);
			}
		}
	};
#endif

}


#endif


