// selection.h

#ifndef __SELECTION_H
#define __SELECTION_H

/*
Roulette: requires RealFitness. By default (?) all population can reproduce (excluding those declared unfit), but will do
so proportionally to their fitness.

Note: could do kind of roulette with SortableFitness, whereby a parameter specifies how much more likely the most fit is to 
reproduce than the least fit. This might in some cases be better even when RealFitness is available, as it doesn't rely on the
ability of the fitness function to produce sensibly distributed fitnesses.

Population proportional: a parameter specifies the proportion of the (not unfit) population that will pass on their genes.
For Sortable fitness, this is simply taken from the top of the sorted genomes. For ComparableFitness it can be approximated by
tournament selection. Will need to define how many children are born from each individual - simplest method would be to give all
(ReqChildren / SelectedPopSize) children, and then give an extra child to the (ReqChildren % SelectedPopSize ) most fit.

To move:
Composite fitness function for RealFitness:
f.set_linear_comb(g1, double p1, g2, double p2, g3 ...)
f.set_product(g1, g2)
	- for the above, must be some normalisation function that can be applied to restore distribution.
	ie. if g1 and g2 are normally (or however seems best) distributed about a mean of 0.5 with some std dev,
	f will have a mean of 0.25 and be squashed down towards 0, which is not ideal (at least, not for roulette selection)
*/

/*
A selection class provides the implementation of the selection process whereby a subset of a population is chosen to pass on genes.

Must provide:

template < typename OutputIterator >
void select_individuals(size_t groups, OutputIterator dest, size_t group_sz = 1)

group_sz: the number of parents from which offspring are formed.
*/


#endif


