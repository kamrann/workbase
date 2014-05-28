// shared_components.h

#ifndef __RTP_PHYS_SHARED_OBJ_COMPONENTS_H
#define __RTP_PHYS_SHARED_OBJ_COMPONENTS_H

#include "ga/objective_fn_data_component.h"


namespace rtp_phys {

	struct timesteps_ofd: public ofd_component
	{
		size_t timesteps;

		timesteps_ofd(): timesteps(0)
		{}

		template < typename agent_decision, typename agent_state, typename envt_state, typename ofdata >
		static inline void update(agent_decision const& dec, agent_state const& agent_st, envt_state const& envt_st, ofdata& ofd)
		{
			++ofd.timesteps;
		}
	};

	struct avg_height_ofd: public ofd_component
	{
		typedef mpl::vector< timesteps_ofd > dependencies;

		float avg_height;

		avg_height_ofd(): avg_height(0.0f)
		{}

		template < typename agent_decision, typename agent_state, typename envt_state, typename ofdata >
		static inline void update(agent_decision const& dec, agent_state const& agent_st, envt_state const& envt_st, ofdata& ofd)
		{
			float height = agent_st.body->get_position().y;
			ofd.avg_height += height;
		}

		template < typename ofdata >
		static inline void finalise(ofdata& ofd)
		{
			ofd.avg_height /= ofd.timesteps;
		}
	};

	template < int Reference >	// TODO: If want to keep this as templated, replace int with ratio to allow non-integer values
	struct avg_y_delta_ofd: public ofd_component
	{
		typedef mpl::vector< timesteps_ofd > dependencies;

		float avg_y_delta;

		avg_y_delta_ofd(): avg_y_delta(0.0f)
		{}

		template < typename agent_decision, typename agent_state, typename envt_state, typename ofdata >
		static inline void update(agent_decision const& dec, agent_state const& agent_st, envt_state const& envt_st, ofdata& ofd)
		{
			float y = agent_st.body->get_position().y;
			ofd.avg_y_delta += std::abs(y - Reference);
		}

		template < typename ofdata >
		static inline void finalise(ofdata& ofd)
		{
			ofd.avg_y_delta /= ofd.timesteps;
		}
	};

/*
	struct avg_tilt_ofd: public ofd_component
	{
		typedef mpl::vector< timesteps_ofd > dependencies;

		float avg_tilt;

		avg_tilt_ofd(): avg_tilt(0.0f)
		{}

		template < typename agent_decision, typename agent_state, typename envt_state, typename ofdata >
		static inline void update(agent_decision const& dec, agent_state const& agent_st, envt_state const& envt_st, ofdata& ofd)
		{
			float angle = agent_st.body->get_orientation();
			ofd.avg_tilt += std::abs(angle);
		}

		template < typename ofdata >
		static inline void finalise(ofdata& ofd)
		{
			ofd.avg_tilt /= ofd.timesteps;
		}
	};
*/
	struct avg_ke_ofd: public ofd_component
	{
		typedef mpl::vector< timesteps_ofd > dependencies;

		float avg_ke;

		avg_ke_ofd(): avg_ke(0.0f)
		{}

		template < typename agent_decision, typename agent_state, typename envt_state, typename ofdata >
		static inline void update(agent_decision const& dec, agent_state const& agent_st, envt_state const& envt_st, ofdata& ofd)
		{
			ofd.avg_ke += agent_st.body->get_kinetic_energy();
		}

		template < typename ofdata >
		static inline void finalise(ofdata& ofd)
		{
			ofd.avg_ke /= ofd.timesteps;
		}
	};

}


#endif


