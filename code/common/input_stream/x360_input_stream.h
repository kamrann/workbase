// x360_input_stream.h

#ifndef __WB_X360_INPUT_STREAM_H
#define __WB_X360_INPUT_STREAM_H

#include "input_stream.h"

#include <Windows.h>
#include <xinput.h>

#include <boost/optional.hpp>

#include <map>


namespace input {

	class x360_input_stream:
		public input_stream
	{
	public:
		x360_input_stream();

		bool connect_to_controller(boost::optional< size_t > c_idx = boost::none);
		void disconnect();

	public:
		virtual void update() override;
		virtual input_stream_access lock() const override;

	protected:
		struct axis2d_pos
		{
			double x, y;
		};

		axis2d_pos scale_axis_2d(
			int x, int y,
			int max,
			int deadzone);
		double scale_positive_axis_1d(
			int x,
			int x_max,
			int deadzone);

	protected:
		boost::optional< size_t > controller_id_;
		std::map< unsigned int, btn_element_id > digital_btn_mp_;

		enum x360_axes {
			LeftThumbX,
			LeftThumbY,
			RightThumbX,
			RightThumbY,
			LeftTrigger,
			RightTrigger,
		};

		std::map< x360_axes, axis_element_id > axis_mp_;
	};

}


#endif

