// x360_input_stream.cpp

#include "x360_input_stream.h"


namespace input {

	x360_input_stream::x360_input_stream()
	{
		digital_btn_mp_[XINPUT_GAMEPAD_A] = register_button("A");
		digital_btn_mp_[XINPUT_GAMEPAD_B] = register_button("B");
		digital_btn_mp_[XINPUT_GAMEPAD_X] = register_button("X");
		digital_btn_mp_[XINPUT_GAMEPAD_Y] = register_button("Y");
		
		digital_btn_mp_[XINPUT_GAMEPAD_DPAD_LEFT] = register_button("D-Pad-Left");
		digital_btn_mp_[XINPUT_GAMEPAD_DPAD_RIGHT] = register_button("D-Pad-Right");
		digital_btn_mp_[XINPUT_GAMEPAD_DPAD_UP] = register_button("D-Pad-Up");
		digital_btn_mp_[XINPUT_GAMEPAD_DPAD_DOWN] = register_button("D-Pad-Down");

		digital_btn_mp_[XINPUT_GAMEPAD_LEFT_SHOULDER] = register_button("Left-Shoulder");
		digital_btn_mp_[XINPUT_GAMEPAD_RIGHT_SHOULDER] = register_button("Right-Shoulder");

		digital_btn_mp_[XINPUT_GAMEPAD_LEFT_THUMB] = register_button("Left-Thumb-Btn");
		digital_btn_mp_[XINPUT_GAMEPAD_RIGHT_THUMB] = register_button("Right-Thumb-Btn");

		digital_btn_mp_[XINPUT_GAMEPAD_BACK] = register_button("Back");
		digital_btn_mp_[XINPUT_GAMEPAD_START] = register_button("Start");


		axis_mp_[x360_axes::LeftThumbX] = register_axis("Left-Thumb-X");
		axis_mp_[x360_axes::LeftThumbY] = register_axis("Left-Thumb-Y");
		axis_mp_[x360_axes::RightThumbX] = register_axis("Right-Thumb-X");
		axis_mp_[x360_axes::RightThumbY] = register_axis("Right-Thumb-Y");
		axis_mp_[x360_axes::LeftTrigger] = register_axis("Left-Trigger");
		axis_mp_[x360_axes::RightTrigger] = register_axis("Right-Trigger");


		initialize();
	}

	bool x360_input_stream::connect_to_controller(boost::optional< size_t > c_idx)
	{
		if(controller_id_)
		{
			if(!c_idx || *c_idx == *controller_id_)
			{
				return true;
			}

			disconnect();
		}

		if(c_idx && *c_idx >= XUSER_MAX_COUNT)
		{
			return false;
		}

		XINPUT_STATE state;
		ZeroMemory(&state, sizeof(XINPUT_STATE));

		for(unsigned long idx = 0; idx < XUSER_MAX_COUNT; ++idx)
		{
			if(c_idx && *c_idx != idx)
			{
				continue;
			}

			if(XInputGetState(idx, &state) == ERROR_SUCCESS)
			{
				controller_id_ = idx;
				break;
			}
		}

		return (bool)controller_id_;
	}

	void x360_input_stream::disconnect()
	{
		controller_id_ = boost::none;
	}

	x360_input_stream::axis2d_pos x360_input_stream::scale_axis_2d(
		int x, int y,
		int max,
		int deadzone
		)
	{
		auto sqr_mag = (double)x * x + (double)y * y;
		if(sqr_mag > (double)deadzone * deadzone)
		{
			auto mag = std::min< double >(std::sqrt(sqr_mag), max);
			float normX = x / mag;
			float normY = y / mag;
			
			mag -= deadzone;
			mag /= (max - deadzone);

			return{ normX * mag, normY * mag };
		}
		else
		{
			return{ 0.0, 0.0 };
		}
	}

	double x360_input_stream::scale_positive_axis_1d(
		int x,
		int x_max,
		int deadzone
		)
	{
		if(x > deadzone)
		{
			return (double)(x - deadzone) / (x_max - deadzone);
		}
		else
		{
			return 0.0;
		}
	}

	void x360_input_stream::update()
	{
		if(!controller_id_)
		{
			return;
		}

		XINPUT_STATE state;
		ZeroMemory(&state, sizeof(XINPUT_STATE));

		if(XInputGetState(*controller_id_, &state) != ERROR_SUCCESS)
		{
			// todo: throw?
			disconnect();
			return;
		}

		for(auto const& btn : digital_btn_mp_)
		{
			bool pressed = (state.Gamepad.wButtons & btn.first) != 0;
			auto elem_id = btn.second;
			btn_element_states_[elem_id.idval].st = pressed ?
				btn_element_state::ButtonState::Pressed :
				btn_element_state::ButtonState::Unpressed;
		}

		auto left_thumb_pos = scale_axis_2d(
			state.Gamepad.sThumbLX,
			state.Gamepad.sThumbLY,
			32767,
			XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE
			);
		axis_element_states_[axis_mp_.at(x360_axes::LeftThumbX).idval].value = left_thumb_pos.x;
		axis_element_states_[axis_mp_.at(x360_axes::LeftThumbY).idval].value = left_thumb_pos.y;

		auto right_thumb_pos = scale_axis_2d(
			state.Gamepad.sThumbRX,
			state.Gamepad.sThumbRY,
			32767,
			XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE
			);
		axis_element_states_[axis_mp_.at(x360_axes::RightThumbX).idval].value = right_thumb_pos.x;
		axis_element_states_[axis_mp_.at(x360_axes::RightThumbY).idval].value = right_thumb_pos.y;

		auto left_trigger_pos = scale_positive_axis_1d(
			state.Gamepad.bLeftTrigger,
			255,
			XINPUT_GAMEPAD_TRIGGER_THRESHOLD
			);
		axis_element_states_[axis_mp_.at(x360_axes::LeftTrigger).idval].value = left_trigger_pos;

		auto right_trigger_pos = scale_positive_axis_1d(
			state.Gamepad.bRightTrigger,
			255,
			XINPUT_GAMEPAD_TRIGGER_THRESHOLD
			);
		axis_element_states_[axis_mp_.at(x360_axes::RightTrigger).idval].value = right_trigger_pos;
	}

	input_stream_access x360_input_stream::lock() const
	{
		// Assume updating thread and locking (data accessing) thread always same, in which
		// case no synchronization necessary.
		return input_stream::lock();
	}

}



