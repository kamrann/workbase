// activation_functions.h

#ifndef __WB_NN_ACTIVATION_FUNCTIONS_CONCEPT_H
#define __WB_NN_ACTIVATION_FUNCTIONS_CONCEPT_H

//#include <boost/variant.hpp>

#include <sstream>
#include <limits>
#include <tuple>


namespace nnet {

	template <
		typename ActionEnum,
		ActionEnum
	>
	struct enum_action_defn
	{
		// Specialize and define: typedef ... return_type;
	};

	template <
		typename Action,
		typename... Elements
	>
	class enum_type
	{
	public:
		enum { NumElements = sizeof...(Elements) };

	protected:
		typedef std::tuple< Elements... > elem_tuple_t;

//		typedef boost::variant< Elements... > enum_value_t;
		struct enum_value_t
		{
			int internal_val;
		};

		/**** Retrieve type at a given index ****/
		template <
			int Index
		>
		struct element_at
		{
			typedef typename std::tuple_element< Index, elem_tuple_t >::type type;
		};

		/**** Retrieve index for a given type ****/
		// Usage is index_of< target_type, list... >
		// Below specialization takes care of all cases except when list... is empty, ie. failure
		// So primary template deals with that by not being defined.
		template <
			typename... Types
		>
		struct index_of
		{
			enum { value = 0 };	// irrelevant, should never be used
		};

		template <
			typename Type,
			typename Head,
			typename... Tail
		>
		struct index_of< Type, Head, Tail... >
		{
			// TODO: template alternative to conditional expression so that dont need to define
			// useless value member of primary template
			enum {
				value =
				is_same < Type, Head >::value ?
				0
				:
				(1 + index_of< Type, Tail... >::value)
			};
		};

		/**** Dispatching code ****/
		template <
			int value,
			int count,	// TODO: Don't think this should be needed, see note below
			Action action,
			typename... Args
		>
		struct dispatcher
		{
			static inline auto dispatch(enum_value_t const rt_value, Args... args)
				-> typename enum_action_defn< Action, action >::return_type
			{
				switch(rt_value.internal_val)//.which())
				{
					case value:
					return typename element_at< value >::type::do_action< action >(args...);

					default:
					return dispatcher< value + 1, count, action, Args... >::dispatch(rt_value, args...);
				}
			}
		};

		// Terminate dispatcher when value == count
		// NOTE: Omitting count template parameter from dispatcher class template, and specializing below using
		// < NumElements, action, Args... > doesn't work - still attempts to instantiate primary template,
		// despite static_assert verifying that NumElements holds correct value. Replacing NumElements with 
		// a hard-coded value works however. Compiler bug???
		template <
			int count,
			Action action,
			typename... Args
		>
		struct dispatcher< count, count, action, Args... >
		{
			// TODO: Hack. Need to terminate the recursion at the last valid enum value, not beyond it
			static inline auto dispatch(enum_value_t, Args...)
				-> typename enum_action_defn< Action, action >::return_type
			{
				return{};
			}
		};

		/************************************************/
	public:
		// Get runtime enum value of the given element type
		template <
			typename Elem
		>
		static inline enum_value_t val()
		{
			return enum_value_t{ index_of< Elem, Elements... >::value };
		}

		// Iterator for iterating over the entire enum range
		class iterator:
			public std::iterator< std::forward_iterator_tag, const enum_value_t >
		{
		public:
			iterator& operator++ ()
			{
				++m_index;
				return *this;
			}

			iterator operator++ (int)
			{
				iterator tmp(*this);
				operator++();
				return tmp;
			}

			bool operator== (const iterator& rhs)
			{
				return m_index == rhs.m_index;
			}

			bool operator!= (const iterator& rhs)
			{
				return !operator==(rhs);
			}

			value_type operator* ()
			{
				return enum_value_t{ m_index };
			}

		private:
			iterator(int index): m_index(index)
			{}

		private:
			int m_index;

			friend class enum_type;
		};

		static inline iterator begin()
		{
			return iterator{ 0 };
		}

		static inline iterator end()
		{
			return iterator{ NumElements };
		}

		// Invoke action on the given runtime enum value
		template <
			Action action,
			typename... Args
		>
		static inline auto do_action(enum_value_t const rt_value, Args... args)
		-> typename enum_action_defn< Action, action >::return_type
		{
			// TODO: Would dispatching via boost::static_visitor work, and be equally efficient?
			return dispatcher< 0, NumElements, action, Args... >::dispatch(rt_value, args...);
		}
	};


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


