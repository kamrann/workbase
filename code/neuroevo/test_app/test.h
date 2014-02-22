// test.h

#ifndef __TEST_APP_H
#define __TEST_APP_H


template < typename D >
struct traits;

template < typename T >
struct Base
{
	typedef T derived_t;
	typedef typename traits< derived_t >::type type_in_base;
};


template < typename X >
struct Derived: public Base< Derived< X > >
{
	typedef typename traits< Derived >::type type;
};

template < typename X >
struct traits< Derived< X > >
{
	typedef X type;
};


#endif


