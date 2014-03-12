// pareto.h

#ifndef __PARETO_H
#define __PARETO_H

#include <vector>
#include <set>


template < typename Comp >
class pareto
{
private:
	typedef Comp comp_t;

	std::vector< comp_t > components;

public:
	pareto()
	{}

	inline void push_component(comp_t const& c)
	{
		components.push_back(c);
	}

private:
/*	template < typename Ftr >
	inline bool for_each_component(Ftr& ftr)
	{
		for(comp_t& c: components)
		{
			if(!ftr(c))
			{
				return false;
			}
		}
		return true;
	}
*/
public:
	inline comp_t& operator[] (size_t i)
	{
		return components[i];
	}

	inline comp_t const& operator[] (size_t i) const
	{
		return components[i];
	}

	inline bool dominates(pareto const& other) const
	{
		bool greater_component = false;
		for(size_t i = 0; i < components.size(); ++i)
		{
			if(components[i] < other.components[i])
			{
				return false;
			}
			greater_component = greater_component || (components[i] > other.components[i]);
		}
		return greater_component;
	}

	inline bool dominated_by(pareto const& other) const
	{
		return other.dominates(*this);
	}

	inline bool same_rank(pareto const& other) const
	{
		return !dominates(other) && !dominated_by(other);
	}

	inline bool operator> (pareto const& other) const
	{
		return dominates(other);
	}

	inline bool operator< (pareto const& other) const
	{
		return dominated_by(other);
	}

	inline bool operator== (pareto const& other) const
	{
		return same_rank(other);
	}

	inline bool operator>= (pareto const& other) const
	{
		return !dominated_by(other);
	}

	inline bool operator<= (pareto const& other) const
	{
		return !dominates(other);
	}

public:
	template < typename Iterator, typename RankIterator >
	static void calc_ranks(Iterator first, Iterator last, RankIterator out_ranks)
	{
		// Preprocess domination matrix (oh yes!)
		typedef std::pair< Iterator, Iterator > pair_t;
		std::map< pair_t, bool > dominated_by;
		for(Iterator it1 = first; it1 != last; ++it1)
		{
			for(Iterator it2 = first; it2 != last; ++it2)
			{
				if(it1 == it2)
				{
					continue;
				}

				dominated_by[pair_t(it1, it2)] = it1->dominated_by(*it2);
			}
		}

		std::set< Iterator > already_ranked;
		size_t rank = 0;
		while(already_ranked.size() < last - first)
		{
			std::set< Iterator > non_dominated;
			for(Iterator it = first; it != last; ++it)
			{
				if(already_ranked.find(it) != already_ranked.end())
				{
					continue;
				}

				bool is_dominated = false;
				for(Iterator other = first; other != last; ++other)
				{
					if(other == it || already_ranked.find(other) != already_ranked.end())
					{
						continue;
					}

					if(dominated_by[pair_t(it, other)])
					{
						is_dominated = true;
						break;
					}
				}

				if(!is_dominated)
				{
					// *it is not dominated by any other still to be ranked value
					non_dominated.insert(it);
				}
			}

			if(non_dominated.empty())
			{
				// Non of the remaining values dominates another, so all these values belong in the last rank
				for(Iterator it = first; it != last; ++it)
				{
					if(already_ranked.find(it) == already_ranked.end())
					{
						out_ranks[it - first] = rank;
					}
				}
				return;
			}
			else
			{
				// Assign rank to every value not dominated in the remaining group, then mark as already ranked
				for(Iterator it: non_dominated)
				{
					out_ranks[it - first] = rank;
					already_ranked.insert(it);
				}
				++rank;
			}
		}
	}
};


#endif


