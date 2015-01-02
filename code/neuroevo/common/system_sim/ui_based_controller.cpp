// ui_based_controller.cpp

#include "ui_based_controller.h"


namespace sys
{
	ui_based_controller::ui_based_controller(size_t num_effectors):
		num_effectors_{ num_effectors }
	{

	}

	void ui_based_controller::set_input_stream(std::shared_ptr< input::input_stream const > strm)
	{
		in_strm_ = strm;
	}

	void ui_based_controller::bind_button(input::btn_element_id btn, unsigned long effector)
	{
		bound_btns_[effector] = btn;
	}

	void ui_based_controller::bind_axis(input::axis_element_id axis, unsigned long effector)
	{
		bound_axes_[effector] = axis;
	}

	void ui_based_controller::auto_bind_all()
	{
		// todo:
	}

	i_controller::input_id_list_t ui_based_controller::get_input_ids() const
	{
		return input_id_list_t{};
	}

	i_controller::output_list_t ui_based_controller::process(input_list_t const&)
	{
		auto input_data = in_strm_->lock();

		output_list_t results;
		for(size_t eff = 0; eff < num_effectors_; ++eff)
		{
			results.push_back(get_activation(eff, input_data));
		}
		return results;
	}

}


