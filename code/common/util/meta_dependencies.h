// meta_dependencies.h

#ifndef __META_DEPENDENCIES_H
#define __META_DEPENDENCIES_H

#include <boost/mpl/vector.hpp>
#include <boost/mpl/fold.hpp>
#include <boost/mpl/back_inserter.hpp>
#include <boost/mpl/copy.hpp>
#include <boost/mpl/push_back.hpp>
#include <boost/mpl/lambda.hpp>
#include <boost/mpl/if.hpp>
#include <boost/mpl/apply_wrap.hpp>
#include <boost/mpl/contains.hpp>
#include <boost/mpl/is_sequence.hpp>

#include <boost/utility/enable_if.hpp>


namespace mpl = boost::mpl;


namespace meta_util {

	namespace detail {

		// Tags to mark whether a type has dependencies, or is itself a dependency component
		struct has_deps {};
		struct is_dep_component: public has_deps {};	// Implies also has_deps (may be 0 deps)
	}

	// Base class for all types with dependencies
	struct has_dependencies_base
	{
		typedef detail::has_deps dep_tag;

		// Default to no dependencies. This can be overridden by "typedef X dependencies;" in derived struct.
		typedef mpl::vector0<> dependencies;
	};

	// Base class for all types which are dependency components
	struct dependency_component_base
	{
		typedef detail::is_dep_component dep_tag;
	};


	// Forward declaration of a metafunction to unroll dependencies
	template < typename T, typename Enable = void >
	struct unique_dependencies;
	// NOTE: This will lead to a "use of undefined type" compiler error when T is neither a type list, nor does it have one of the above tags

	namespace detail {

		// Implementation for any type which has dependencies.
		// Assume that T has an MPL sequence member "dependencies" and delegate to mpl sequence implementation.
		template < typename T >
		struct unique_dependencies_has_deps:
			public unique_dependencies< typename T::dependencies >
		{};

		// Implementation for any type which is itself a dependency component.
		// Any component must also have a (potentially empty) sequence of dependencies, so delegate to that first by inheritance.
		template < typename T >
		struct unique_dependencies_is_comp:
			public unique_dependencies_has_deps< T >
		{
			typedef T t;
			typedef unique_dependencies_has_deps< T > base_t;

			// And then add T itself as an dependency component
			typedef typename mpl::push_back< typename base_t::type, t >::type type;
		};
	}

	// Partial specialisation for types that have dependencies
	template < typename T >
	struct unique_dependencies< T, typename boost::enable_if< boost::is_same< typename T::dep_tag, detail::has_deps > >::type >:
		public detail::unique_dependencies_has_deps< T >
	{};

	// Partial specialisation for types which are dependency components
	template < typename T >
	struct unique_dependencies< T, typename boost::enable_if< boost::is_same< typename T::dep_tag, detail::is_dep_component > >::type >:
		public detail::unique_dependencies_is_comp< T >
	{};

	// T is an MPL sequence. Assume that its elements are all ofd components or have ofd dependencies.
	template < typename T >
	struct unique_dependencies< T, typename boost::enable_if< typename mpl::is_sequence< T > >::type >
	{
		typedef T seq_t;

		// Need a metafunction class for recursive definition below.
		struct ud_mfc
		{
			template < typename _T >
			struct apply
			{
				typedef unique_dependencies< _T > type;
			};
		};

		typedef typename mpl::fold<
			seq_t,
			mpl::vector0<>,
			mpl::lambda<
				mpl::copy<
					typename mpl::apply_wrap1< typename mpl::lambda< ud_mfc >::type, mpl::_2 >::type,
					mpl::back_inserter< mpl::_1 >
				>
			>
		>::type combined_deps;

		// Remove duplicates
		typedef typename mpl::fold<
			combined_deps,
			mpl::vector<>,
			mpl::if_<
				mpl::contains< mpl::_1, mpl::_2 >,
				mpl::_1,
				mpl::push_back< mpl::_1, mpl::_2 >
			>
		>::type type;
	};
}

/*
template < typename T, typename Enable = void >
struct unique_dependencies
{
	typedef T t;
	typedef typename t::dependencies t_dep;

	struct ud_mfc
	{
		template < typename _T >
		struct apply
		{
			typedef unique_dependencies< _T > type;
		};
	};

	typedef typename mpl::fold<
		t_dep,
		mpl::vector0<>,
		mpl::lambda<
			mpl::copy<
				typename mpl::apply_wrap1< typename mpl::lambda< ud_mfc >::type, _2 >::type,
				mpl::back_inserter< _1 >
			>
		>
	>::type dep_types;

	// Remove duplicates
	typedef typename mpl::fold<
		dep_types,
		mpl::vector<>,
		mpl::if_<
			mpl::contains< _1, _2 >,
			_1,
			mpl::push_back< _1, _2 >
		>
	>::type unique_dep_types;

	// Plus T
	typedef typename mpl::push_back< unique_dep_types, t >::type type;
};

template < typename T >
struct unique_dependencies< T, typename boost::enable_if< mpl::empty< typename T::dependencies > >::type >
{
	typedef mpl::vector1< T > type;
};
*/


#endif


