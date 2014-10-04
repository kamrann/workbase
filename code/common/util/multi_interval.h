// multi_interval.h

#ifndef __WB_MULTI_INTERVAL_H
#define __WB_MULTI_INTERVAL_H

#include <boost/numeric/interval.hpp>

#include <set>


namespace boost {
	namespace numeric {

		template < typename T, typename Policies >
		inline bool is_positively_bounded(interval< T, Policies > const& itv)
		{
			typedef typename interval< T, Policies >::traits_type traits_t;
			typedef typename traits_t::checking checking_t;
			return itv.upper() != checking_t::pos_inf();
		}

		template < typename T, typename Policies >
		inline bool is_negatively_bounded(interval< T, Policies > const& itv)
		{
			typedef typename interval< T, Policies >::traits_type traits_t;
			typedef typename traits_t::checking checking_t;
			return itv.lower() != checking_t::neg_inf();
		}

		template < typename T, typename Policies >
		inline bool is_bounded(interval< T, Policies > const& itv)
		{
			return is_positively_bounded(itv) && is_negatively_bounded(itv);
		}

	}
}


namespace bint = boost::numeric;

template <
	typename T,
	typename Policies//,
//	template < typename, typename > class Approximator = exact_impl
>
class multi_interval_base
{
public:
	typedef bint::interval< T, Policies > interval;
	typedef T base_type;

protected:
	// Since sub intervals will by definition not overlap, they can be ordered by a single bound only
	struct sub_ordering
	{
		inline bool operator() (interval const& itv1, interval const& itv2) const
		{
			return itv1.upper() < itv2.upper();
		}
	};

	//	typedef std::set< interval, sub_ordering > sub_interval_set;

public:
	typedef std::set< interval, sub_ordering > sub_interval_set;
	//
	typedef typename sub_interval_set::iterator iterator;
	typedef typename sub_interval_set::const_iterator const_iterator;
};


template < typename T, typename Policies >
struct exact_impl;


template <
	typename T,
	typename Policies,
	template < typename, typename > class Approximator = exact_impl
>
class multi_interval:
	public multi_interval_base< T, Policies >
{
public:
	typedef Approximator< T, Policies > approximator;

public:
	multi_interval(): m_subs{}
	{}
	explicit multi_interval(base_type val)//: m_subs{ interval{ val } }
		: multi_interval(interval{ val })
	{
		//m_approx.insert(m_subs, interval{ val }, std::end(m_subs));
	}
	explicit multi_interval(interval const& itv)//: m_subs{ itv }
	{
		m_approx.insert(m_subs, itv, std::end(m_subs));
	}
	explicit multi_interval(std::initializer_list< interval > il)
	{
		for(auto const& itv : il)
		{
			insert(itv);
		}
	}
	explicit multi_interval(std::initializer_list< multi_interval > il)
	{
		for(auto const& mitv : il)
		{
			insert(mitv);
		}
	}

	// The following two static methods are to avoid ambiguity when constructing from individual values.
	// This fills the multi interval with a list of individual values.
	static inline multi_interval from_set(std::initializer_list< base_type > il)
	{
		multi_interval mitv;
		for(auto const& val : il)
		{
			mitv.insert(val);
		}
		return mitv;
	}

	// Whereas this takes two individual values and treats them as the bounds of an interval.
	static inline multi_interval from_interval(base_type lower, base_type upper)
	{
		return multi_interval{ interval{ lower, upper } };
	}

public:
	static inline base_type pos_infinity()
	{
		return interval::traits_type::checking::pos_inf();
	}

	static inline base_type neg_infinity()
	{
		return interval::traits_type::checking::neg_inf();
	}

	static inline multi_interval whole()
	{
		return multi_interval{ interval::whole() };
	}

public:
	inline iterator begin()
	{
		return std::begin(m_subs);
	}

	inline iterator end()
	{
		return std::end(m_subs);
	}

	inline const_iterator begin() const
	{
		return std::begin(m_subs);
	}

	inline const_iterator end() const
	{
		return std::end(m_subs);
	}

	inline bool is_empty() const
	{
		return m_subs.empty();
	}

	inline bool is_single_interval() const
	{
		return m_subs.size() == 1;
	}

	inline bool is_single_value() const
	{
		return is_single_interval() && bint::singleton(*begin());
	}

	inline bool is_positively_bounded() const
	{
		if(is_empty())
		{
			return true;
		}
		
		auto it = end();
		--it;
		return bint::is_positively_bounded(*it);
	}

	inline bool is_negatively_bounded() const
	{
		return is_empty() || bint::is_negatively_bounded(*begin());
	}

	inline bool is_bounded() const
	{
		return is_positively_bounded() && is_negatively_bounded();
	}

	inline interval const& lowest() const
	{
		return *m_subs.begin();
	}

	inline interval const& highest() const
	{
		return *std::prev(m_subs.end());
	}

	// These assume non-empty multi interval
	inline base_type lower_bound() const
	{
		return lowest().lower();
	}

	inline base_type upper_bound() const
	{
		return highest().upper();
	}

	inline void set_empty()
	{
		//m_subs.clear();
		m_approx.remove(m_subs, std::begin(m_subs), std::end(m_subs));
	}

	inline iterator insert(interval const& itv)
	{
		// Find the location in our sub interval list of the first sub-interval with an upper bound
		// greater than or equal to itv's lower bound.
		auto it_first = m_subs.lower_bound(interval(itv.lower()));
		// Now starting from that location, find all subs with a lower bound less than or equal to itv's upper
		// (These are the ones that are overlapping)
		auto it = it_first;
		base_type existing_upper;
		while(it != end() && it->lower() <= itv.upper())
		{
			existing_upper = it->upper();
			++it;
		}

		if(it != it_first)
		{
			// itv overlaps one or more of our sub intervals - those in [it_first, it)
			// Merge all these with itv to create a single interval
			auto const lower = std::min(it_first->lower(), itv.lower());
			auto const upper = std::max(existing_upper, itv.upper());
			// Remove those that were overlapped
			//m_subs.erase(it_first, it);
			m_approx.remove(m_subs, it_first, it);
			// And then insert the new interval (should go before the first element that was not overlapped/end)
			//return m_subs.insert(it, interval{ lower, upper });
			return m_approx.insert(m_subs, interval{ lower, upper }, it);
		}
		else
		{
			// no overlap, insert itv
			//return m_subs.insert(it_first, itv);
			return m_approx.insert(m_subs, itv, it_first);
		}
	}

	inline void insert(multi_interval const& r_mitv)
	{
		// This iterator marks the first location in our own sub list at which we can possible find overlap
		// of the next sub interval from r_mitv
		iterator it_first = begin();
		// Iterate over sub intervals of r_mitv
		for(auto r_it = std::begin(r_mitv); r_it != std::end(r_mitv); ++r_it)
		{
			// Current sub-interval from r_mitv
			auto const& r_itv = *r_it;
			// Find the location in our sub interval list of the first sub-interval with an upper bound
			// greater than or equal to r_itv's lower bound.
			// TODO: This sucks, to make use of the optimization that we know we can start the search from it_first,
			// we have to fall back on the linear version of lower_bound. Should test this, may well be the case that
			// forgetting the starting point optimization and just using set::lower_bound() is faster.
//			it_first = std::lower_bound(it_first, end(), interval(r_itv.lower()), sub_ordering());
			it_first = m_subs.lower_bound(interval(r_itv.lower()));
			// Now starting from that location, find all subs with a lower bound less than or equal to r_itv's upper
			// (These are the ones that are overlapping)
			auto it = it_first;
			base_type existing_upper;
			while(it != end() && it->lower() <= r_itv.upper())
			{
				existing_upper = it->upper();
				++it;
			}

			if(it != it_first)
			{
				// r_itv overlaps one or more of our sub intervals - those in [it_first, it)
				// Merge all these with r_itv to create a single interval
				auto const lower = std::min(it_first->lower(), r_itv.lower());
				auto const upper = std::max(existing_upper, r_itv.upper());
				// Remove those that were overlapped
				//m_subs.erase(it_first, it);
				m_approx.remove(m_subs, it_first, it);
				// And then insert the new interval (should go before the first element that was not overlapped/end)
				//m_subs.insert(it, interval{ lower, upper });
				m_approx.insert(m_subs, interval{ lower, upper }, it);
			}
			else
			{
				// no overlap, insert r_itv
				//m_subs.insert(it_first, r_itv);
				m_approx.insert(m_subs, r_itv, it_first);
			}

			// The next sub from r_mitv cannot overlap anything before it
			it_first = it;
		}
	}

	inline multi_interval& operator|= (interval const& rhs)
	{
		insert(rhs);
		return *this;
	}

	inline multi_interval& operator|= (multi_interval const& rhs)
	{
		insert(rhs);
		return *this;
	}

	inline multi_interval operator* (base_type rhs) const
	{
		multi_interval result;
		for(auto& sub : m_subs)
		{
			result |= sub * rhs;
		}
		return result;
	}

	inline multi_interval& operator*= (base_type rhs)
	{
		return (*this = *this * rhs);
	}

	inline multi_interval operator+ (interval const& rhs) const
	{
		multi_interval result;
		for(auto const& sub : m_subs)
		{
			result |= sub + rhs;
		}
		return result;
	}
	
	inline multi_interval& operator+= (interval const& rhs)
	{
		return (*this = *this + rhs);
	}

	inline multi_interval operator+ (multi_interval const& rhs) const
	{
		multi_interval result;
		for(auto const& sub : m_subs)
		{
			for(auto const& r_sub : rhs.m_subs)
			{
				result |= sub + r_sub;
			}
		}
		return result;
	}

	inline multi_interval& operator+= (multi_interval const& rhs)
	{
		return (*this = *this + rhs);
	}

private:
	/*
	inline iterator insert_sub(interval const& itv)
	{
		return m_subs.insert(itv);
	}

	static inline multi_interval combine(interval const& itv1, interval const& itv2)
	{
		if(bint::overlap(itv1, itv2))
		{
			return multi_interval{ bint::hull(itv1, itv2) };
		}
		else
		{
			multi_interval result;
			result.insert_sub(itv1);
			result.insert_sub(itv2);
			return result;
		}
	}
	*/

private:
	sub_interval_set m_subs;
	approximator m_approx;
};


template < typename T, typename Policies, template <typename, typename> class Approximator >
inline multi_interval< T, Policies, Approximator > operator| (multi_interval< T, Policies, Approximator > const& multi, bint::interval< T, Policies > const& single)
{
	multi_interval< T, Policies, Approximator > result(multi);
	result |= single;
	return result;
}

template < typename T, typename Policies, template <typename, typename> class Approximator >
inline multi_interval< T, Policies, Approximator > operator| (multi_interval< T, Policies, Approximator > const& lhs, multi_interval< T, Policies, Approximator > const& rhs)
{
	multi_interval< T, Policies, Approximator > result(lhs);
	result |= rhs;
	return result;
}


template < typename T, typename Policies >
struct exact_impl
{
	typedef multi_interval_base< T, Policies > multi_interval_base_t;
	typedef typename multi_interval_base_t::interval interval;
	typedef typename multi_interval_base_t::sub_interval_set sub_interval_set;
	typedef typename sub_interval_set::iterator sub_iterator;

	inline sub_iterator insert(
		sub_interval_set& subs,
		interval const& itv,
		sub_iterator place
		)
	{
		return subs.insert(place, itv);
	}

	inline sub_iterator remove(
		sub_interval_set& subs,
		sub_iterator first,
		sub_iterator last
		)
	{
		return subs.erase(first, last);
	}
};

template < typename T, typename Policies >
struct continuous_impl
{
	typedef multi_interval_base< T, Policies > multi_interval_base_t;
	typedef typename multi_interval_base_t::interval interval;
	typedef typename multi_interval_base_t::sub_interval_set sub_interval_set;
	typedef typename sub_interval_set::iterator sub_iterator;

	inline sub_iterator insert(
		sub_interval_set& subs,
		interval const& itv,
		sub_iterator place
		)
	{
		assert(subs.size() <= 1);
		if(subs.empty())
		{
			return subs.insert(place, itv);
		}
		else
		{
			subs = sub_interval_set{ bint::hull(*subs.begin(), itv) };
			return subs.begin();
		}
	}

	inline sub_iterator remove(
		sub_interval_set& subs,
		sub_iterator first,
		sub_iterator last
		)
	{
		return subs.erase(first, last);
	}
};

template < typename T, typename Policies >
struct limited_impl
{
	typedef multi_interval_base< T, Policies > multi_interval_base_t;
	typedef typename multi_interval_base_t::interval interval;
	typedef typename multi_interval_base_t::sub_interval_set sub_interval_set;
	typedef typename sub_interval_set::iterator sub_iterator;

	inline sub_iterator insert(
		sub_interval_set& subs,
		interval const& itv,
		sub_iterator place
		)
	{
		auto inserted_pos = subs.insert(place, itv);
		if(subs.size() > 256)
		{
			auto min_gap = std::numeric_limits< typename multi_interval_base_t::base_type >::max();
			sub_iterator min_gap_it;
			auto it_prev = std::begin(subs);
			auto it = std::next(it_prev);
			while(it != std::end(subs))
			{
				auto gap = it->lower() - it_prev->upper();
				if(gap < min_gap)
				{
					min_gap = gap;
					min_gap_it = it_prev;
				}

				it_prev = it++;
			}

			auto first = min_gap_it;
			auto second = std::next(first);
			bool merged_addition = inserted_pos == first || inserted_pos == second;
			auto merged = bint::hull(*first, *second);
			place = remove(subs, first, std::next(second));
			auto merged_pos = subs.insert(place, merged);
			inserted_pos = merged_addition ? merged_pos : inserted_pos;
		}

		return inserted_pos;
	}

	inline sub_iterator remove(
		sub_interval_set& subs,
		sub_iterator first,
		sub_iterator last
		)
	{
		return subs.erase(first, last);
	}
};



#endif


