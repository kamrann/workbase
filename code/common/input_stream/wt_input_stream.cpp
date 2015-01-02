// wt_input_stream.cpp

#include "wt_input_stream.h"

#include <Wt/WInteractWidget>


namespace input {

	wt_input_stream::wt_input_stream()
	{
		input_element_name name;

		for(auto k = Wt::Key::Key_A; k <= Wt::Key::Key_Z; k = (Wt::Key)(k + 1))
		{
			auto chr = 'A' + (int)(k - Wt::Key::Key_A);
			name = chr;
			key_mp_[k] = register_button(name);
		}

		for(auto k = Wt::Key::Key_F1; k <= Wt::Key::Key_F12; k = (Wt::Key)(k + 1))
		{
			auto fn_idx = (int)(k - Wt::Key::Key_F1);
			name = "F";
			name += std::to_string(fn_idx + 1);
			key_mp_[k] = register_button(name);
		}

		key_mp_[Wt::Key::Key_Up] = register_button("Up");
		key_mp_[Wt::Key::Key_Down] = register_button("Down");
		key_mp_[Wt::Key::Key_Left] = register_button("Left");
		key_mp_[Wt::Key::Key_Right] = register_button("Right");

		initialize();
	}

	void wt_input_stream::attach_to_widget(Wt::WInteractWidget* wgt)
	{
		key_down_conn_ = wgt->keyWentDown().connect(std::bind([this](Wt::WKeyEvent ev)
		{
			std::lock_guard< std::mutex > lk{ mtx_ };
			auto it = key_mp_.find(ev.key());
			if(it != key_mp_.end())
			{
				auto id = it->second;
				btn_element_states_[id.idval].st = btn_element_state::ButtonState::Pressed;
			}
		}, std::placeholders::_1));

		key_up_conn_ = wgt->keyWentUp().connect(std::bind([this](Wt::WKeyEvent ev)
		{
			std::lock_guard< std::mutex > lk{ mtx_ };
			auto it = key_mp_.find(ev.key());
			if(it != key_mp_.end())
			{
				auto id = it->second;
				btn_element_states_[id.idval].st = btn_element_state::ButtonState::Unpressed;
			}
		}, std::placeholders::_1));
	}

	void wt_input_stream::detach_from_widget(Wt::WInteractWidget* wgt)
	{
		// todo: synchronization issue with this?
		wgt->keyWentDown().disconnect(key_down_conn_);
		wgt->keyWentUp().disconnect(key_up_conn_);
	}

	input_stream_access wt_input_stream::lock() const
	{
		std::lock_guard< std::mutex > lk{ mtx_ };
		return input_stream::lock();
	}

}



