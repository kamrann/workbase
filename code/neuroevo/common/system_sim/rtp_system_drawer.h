// rtp_system_drawer.h

#ifndef __NE_RTP_SYSTEM_DRAWER_H
#define __NE_RTP_SYSTEM_DRAWER_H

#include <boost/thread/mutex.hpp>


namespace Wt {
	class WPainter;
}

namespace sys {

	class i_system_drawer
	{
	public:
		struct options_t
		{
			double zoom;

			options_t():
				zoom(1.0)
			{}
		};

	public:
		virtual bool is_animated() const = 0;
		virtual void draw_system(Wt::WPainter& painter, options_t const& options = options_t()) = 0;
	};

	class animated_system_drawer:
		public i_system_drawer
	{
	public:
		virtual bool is_animated() const override
		{
			return true;
		}

		enum class Result {
			Finished,
			Ongoing,
		};

		// Called once for each state that the system enters
		virtual void start() = 0;
		// Advances animation, and return value signals whether or not current animation is completed
		virtual Result advance() = 0;
	};

	class static_system_drawer:
		public i_system_drawer
	{
	public:
		virtual bool is_animated() const override
		{
			return false;
		}
	};

}


#endif

