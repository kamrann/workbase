// input_stream.cpp

#include "input_stream.h"


namespace input {

	btn_element_id input_stream::register_button(input_element_name name)
	{
		auto id = btn_element_id{ btn_element_names_.size() };
		btn_element_names_.push_back(name);
		return id;
	}

	axis_element_id input_stream::register_axis(input_element_name name)
	{
		auto id = axis_element_id{ axis_element_names_.size() };
		axis_element_names_.push_back(name);
		return id;
	}

	void input_stream::initialize()
	{
		btn_element_states_.clear();
		btn_element_states_.resize(button_count(), btn_element_state{});

		axis_element_states_.clear();
		axis_element_states_.resize(axis_count(), axis_element_state{});
	}

	size_t input_stream::button_count() const
	{
		return btn_element_names_.size();
	}

	size_t input_stream::axis_count() const
	{
		return axis_element_names_.size();
	}

	input_element_name_list_t input_stream::available_buttons() const
	{
		return btn_element_names_;
	}

	input_element_name_list_t input_stream::available_axes() const
	{
		return axis_element_names_;
	}

	input_stream_access input_stream::lock() const
	{
		return input_stream_access{ *this };
	}


	input_stream_access::input_stream_access(input_stream const& strm):
		btn_element_states_(strm.btn_element_states_)
		, axis_element_states_(strm.axis_element_states_)
	{

	}

}


