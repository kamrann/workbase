// thruster.h

#ifndef __THRUSTER_H
#define __THRUSTER_H

#include "util/dimensionality.h"

#include <Eigen/StdVector>

#include <boost/shared_ptr.hpp>

#include <set>
#include <algorithm>


using namespace Eigen;


static const double COOLDOWN_RATE = 1.0;

class thruster
{
public:
	bool					engaged;
//	double					heat;
	double					activity;
	std::vector< double >	activity_buffer;
	size_t					buffer_pos;

public:
	thruster(double expected_freq = 60.0, double cooldown_time = 1.0)
	{
		engaged = false;
//		heat = 0.0;

		activity = 0.0;
		buffer_pos = 0;
		auto buffer_size = (size_t)(cooldown_time * expected_freq);
		activity_buffer.resize(buffer_size, 0.0);
	}

	void engage()
	{
		engaged = true;
//		heat = 1.0;
		activity -= activity_buffer[buffer_pos];
		activity_buffer[buffer_pos] = 1.0;
		activity += 1.0;
		buffer_pos = (buffer_pos + 1) % activity_buffer.size();
	}

	void disengage()
	{
		engaged = false;
	}

	void cool_down(double time)
	{
		engaged = false;
//		heat -= time * COOLDOWN_RATE;
//		heat = std::max(heat, 0.0);

		// TODO: This is assuming constant (or roughly constant) update frequency, as specified in constructor
		activity -= activity_buffer[buffer_pos];
		activity_buffer[buffer_pos] = 0.0;
		buffer_pos = (buffer_pos + 1) % activity_buffer.size();
	}

	bool is_engaged() const
	{
		return engaged;
	}

	bool is_hot() const
	{
		return temperature() > 0.0;
	}

	double temperature() const
	{
//		return heat;
		return activity / activity_buffer.size();
	}
};


class thruster_base
{
public:
	typedef std::vector< bool >	thruster_activation;
};

template< WorldDimensionality dim >
class thruster_config: public thruster_base
{
public:
	typedef DimensionalityTraits< dim >					dim_traits;
	typedef typename dim_traits::position_t				pos_t;
	typedef typename dim_traits::direction_t			lin_dir_t;
	typedef typename dim_traits::angular_direction_t	ang_dir_t;

public:
	struct t_data
	{
		pos_t		pos;
		lin_dir_t	dir;

		EIGEN_MAKE_ALIGNED_OPERATOR_NEW
	};

	std::vector< t_data, Eigen::aligned_allocator< t_data > >	thrusters;

public:
	t_data& operator[] (int const i)
	{
		return thrusters[i];
	}

	t_data const& operator[] (int const i) const
	{
		return thrusters[i];
	}

protected:
	typedef std::set< size_t > t_set;
	
	template < typename Ftr, typename Iterator >
	static void for_each_subset(Iterator s_begin, Iterator s_end, t_set& ss, Ftr& fn)
	{
		if(s_begin == s_end)
		{
			// Termination case, evaluate ftr on enumerated subset
			fn(ss);
			return;
		}

		// Take the first remaining element
		auto elem = *s_begin;
		++s_begin;
		// Without this element included
		for_each_subset(s_begin, s_end, ss, fn);
		// Now with this element
		ss.insert(elem);
		for_each_subset(s_begin, s_end, ss, fn);
		ss.erase(elem);
	}

public:
	template < typename Ftr >
	static void for_each_subset(size_t num_thrusters/*t_set const& s*/, Ftr& fn)
	{
		t_set s;
		for(size_t i = 0; i < num_thrusters; ++i)
		{
			s.insert(i);
		}
		t_set ss;
		for_each_subset(s.begin(), s.end(), ss, fn);
	}

	struct store_null_resultants
	{
		thruster_config const& t_cfg;
		pos_t c_of_mass;
		std::set< thruster_activation > null_sets;

		store_null_resultants(thruster_config const& cfg, pos_t const& com): t_cfg(cfg), c_of_mass(com)
		{}

		inline void operator() (t_set const& ss)
		{
			if(ss.size() >= 2)
			{
				thruster_activation ta(t_cfg.thrusters.size(), false);
				for(size_t t_idx: ss)
				{
					ta[t_idx] = true;
				}
				dim_traits::force_t f;
				dim_traits::torque_t t;
				std::tie(f, t) = t_cfg.calc_resultants(ta, c_of_mass);

				const double eps = 0.0001;
				if(magnitude(f) < eps && magnitude(t) < eps)
				{
					// Considered a null activation subset
					// TODO: Currently storing all supersets of each minimal null subset, which we don't need...
					null_sets.insert(ta);
				}
			}
		}
	};

public:
	inline size_t num_thrusters() const
	{
		return thrusters.size();
	}

	std::pair< lin_dir_t, ang_dir_t >
		calc_resultants(thruster_activation const& ta, pos_t const& c_of_mass = zero_val< pos_t >()) const
	{
		lin_dir_t thrust(zero_val< lin_dir_t >());
		ang_dir_t torque(zero_val< ang_dir_t >());
		for(size_t i = 0; i < thrusters.size(); ++i)
		{
			if(!ta[i])
			{
				continue;
			}

			lin_dir_t force = thrusters[i].dir;	// assuming normalized
			thrust += force;
			torque += dim_traits::torque(thrusters[i].pos - c_of_mass, force);
		}
		return std::pair< lin_dir_t, ang_dir_t >(thrust, torque);
	}
};


template< WorldDimensionality dim >
class thruster_system
{
public:
	typedef thruster_config< dim > config_t;

	template < typename ThrusterT >
	struct t_data
	{
		ThrusterT&								t;
		typename config_t::pos_t const&			pos;
		typename config_t::lin_dir_t const&		dir;

		t_data(typename config_t::t_data const& _d, ThrusterT& _t): t(_t), pos(_d.pos), dir(_d.dir)
		{}
	};

public:
	//config_t const&				cfg;
	boost::shared_ptr< config_t >	cfg;
	std::vector< thruster >			thrusters;

public:
	thruster_system(boost::shared_ptr< config_t > _cfg):
		//config_t const& _cfg):
		cfg(_cfg)
	{
		sync_to_cfg();
	}

	// TODO: this sucks
	void sync_to_cfg()
	{
		thrusters.clear();
		if(cfg)
		{
			thrusters.resize(cfg->num_thrusters());
		}
	}

	inline t_data< thruster > operator[] (int i)
	{
		return t_data< thruster >((*cfg)[i], thrusters[i]);
	}

	inline t_data< thruster const > operator[] (int i) const
	{
		return t_data< thruster const >((*cfg)[i], thrusters[i]);
	}

public:
	std::pair< typename config_t::lin_dir_t, typename config_t::ang_dir_t >
		calc_resultants(typename config_t::pos_t const& c_of_mass = zero_val< config_t::pos_t >()) const
	{
		thruster_base::thruster_activation ta;
		for(thruster const& t: thrusters)
		{
			ta.push_back(t.is_engaged());
		}

		return cfg->calc_resultants(ta, c_of_mass);
	}
};


#endif


