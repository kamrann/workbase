// hard_constraint.h

#ifndef __HARD_CONSTRAINT_H
#define __HARD_CONSTRAINT_H


/*

Must provide:

bool test_satisfied(action_taken)
	// TODO: What is a general contraint a function of??

*/

// TODO: For now implement using virtual method. May be a better way...

template< typename Variables >
class hard_constraint
{
public:
	virtual bool test_satisfied(Variables const& var) const = 0;
};


#endif


