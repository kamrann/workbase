// pareto_ct.h

#ifndef __PARETO_CT_H
#define __PARETO_CT_H

#include <boost/fusion/container/vector.hpp>
#include <boost/fusion/mpl.hpp>
#include <boost/fusion/algorithm/iteration/for_each.hpp>
#include <boost/fusion/algorithm/iteration/fold.hpp>
#include <boost/fusion/view/zip_view.hpp>
#include <boost/fusion/sequence/intrinsic/at_c.hpp>

#include <map>
#include <set>


namespace fus = boost::fusion;


template <
	typename ComponentTypelist
>
class pareto: public fus::result_of::as_vector< ComponentTypelist >::type
{
private:
	typedef pareto this_t;
	typedef fus::vector< this_t const&, this_t const& > zip_seq_t;
	typedef fus::zip_view< zip_seq_t > zip_view_t;

private:
	inline zip_view_t zipped_to(pareto const& other) const
	{
		return zip_view_t(zip_seq_t(*this, other));
	}

	template < typename Ftr >
	void parallel_for_each(pareto const& other, Ftr& ftr) const
	{
		fus::for_each(
			zipped_to(other),
			ftr
			);
	}

	template < typename Ftr >
	typename Ftr::result_type parallel_fold(pareto const& other, Ftr& ftr, typename Ftr::result_type const& initial_st) const
	{
		return fus::fold(
			zipped_to(other),
			initial_st,
			ftr
			);
	}

	// private implementation details
	struct parallel_breakable_for_each_detail
	{
		// shorthand ...
		template < class S >
		struct fusion_size
		{
			static const unsigned value = fus::result_of::size< S >::type::value;
		};

		// classic compile-time counter
		template < unsigned >
		struct uint_ {};

		template < typename Seq, typename Ftr >
		static void impl(Seq&, Ftr&, uint_< fusion_size< Seq >::value >, int)
		{ /* reached the end, do nothing */ }

		template < typename Seq, typename Ftr, unsigned I >
		static void impl(Seq& s, Ftr& ftr, uint_< I >, long)
		{
			if(!ftr(fus::at_c< I >(s)))
			{
				return;
			}
			impl(s, ftr, uint_< I + 1 >(), 0);
		}
	};

	template < class Ftr >
	void parallel_breakable_for_each(pareto const& other, Ftr& ftr) const
	{
		parallel_breakable_for_each_detail::impl(zipped_to(other), ftr, parallel_breakable_for_each_detail::uint_< 0 >(), 0);
	}

private:
	struct dom_ftr
	{
		/*
		struct state
		{
		bool less_component;
		bool greater_component;

		inline state(): less_component(false), greater_component(false)
		{}

		inline bool result() const
		{
		return !less_component && greater_component;
		}
		};

		typedef state result_type;
		*/

		bool less_component;
		bool greater_component;

		dom_ftr(): less_component(false), greater_component(false)
		{}

		template < typename T >		// T will be one of the types from ComponentTypelist
		//inline result_type operator() (state const& st, T const& x) const
		inline bool operator() (T const& x)
		{
			if(fus::at_c< 0 >(x) < fus::at_c< 1 >(x))
			{
				less_component = true;
				// Bail
				return false;
			}
			else
			{
				greater_component = greater_component || fus::at_c< 0 >(x) > fus::at_c< 1 >(x);
				// Keep going
				return true;
			}
		}

		inline bool result() const
		{
			return !less_component && greater_component;
		}
	};

public:
	inline bool dominates(pareto const& other) const
	{
		dom_ftr ftr;
		parallel_breakable_for_each(other, ftr);
		return ftr.result();
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

private:
	template < typename Iterator >
	class dominance_matrix
	{
	private:
		typedef std::pair< Iterator, Iterator > dominance_pair_t;
		typedef std::map< dominance_pair_t, bool > dominance_map_t;

		dominance_map_t mp;

	public:
		dominance_matrix(Iterator first, Iterator last)
		{
			for(Iterator it1 = first; it1 != last; ++it1)
			{
				for(Iterator it2 = first; it2 != last; ++it2)
				{
					if(it1 == it2)
					{
						continue;
					}

					mp[dominance_pair_t(it1, it2)] = it1->dominates(*it2);
				}
			}
		};

		inline bool dominates(Iterator x, Iterator y)
		{
			return mp[dominance_pair_t(x, y)];
		}

		inline bool dominated_by(Iterator x, Iterator y)
		{
			return mp[dominance_pair_t(y, x)];
		}
	};

public:
	// Ranking based on how many other values a given value is dominated by
	template < typename Iterator, typename RankIterator >
	static void calc_dominance_ranks(Iterator first, Iterator last, RankIterator out_ranks)
	{
		// Preprocess domination matrix (oh yes!)
		dominance_matrix< Iterator > dm(first, last);

		// TODO:
	}

	// Ranking based on how many other values a given value dominates
	template < typename Iterator, typename RankIterator >
	static void calc_dominance_counts(Iterator first, Iterator last, RankIterator out_ranks)
	{
		// Preprocess domination matrix (oh yes!)
		dominance_matrix< Iterator > dm(first, last);

		// TODO:
	}

	// Ranking based on dividing all values into different fronts
	template < typename Iterator, typename RankIterator >
	static void calc_dominance_depths(Iterator first, Iterator last, RankIterator out_ranks)
	{
		// Preprocess domination matrix (oh yes!)
		dominance_matrix< Iterator > dm(first, last);

		std::set< Iterator > already_ranked;
		size_t rank = 0;
		size_t count = (size_t)(last - first);
		while(already_ranked.size() < count)
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

					if(dm.dominated_by(it, other))
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


