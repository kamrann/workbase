// ui_based_controller.h

#ifndef __NE_UI_BASED_CONRTOLLER_H
#define __NE_UI_BASED_CONRTOLLER_H

#include "controller.h"
#include "input_stream/input_stream.h"

#include <vector>
#include <map>
#include <memory>


namespace sys
{
	// TODO: Named effectors

	class ui_based_controller:
		public i_controller
	{
	public:
		struct binding_info
		{
			unsigned long effector;	// todo: named
			std::string binding;
		};

		typedef std::vector< binding_info > binding_info_list;

// TODO:		static boost::optional< unsigned long > extract_input_binding(prm::param binding);
//		static std::unique_ptr< interactive_controller > create_instance(prm::param_accessor param_vals);

	public:
		ui_based_controller(size_t num_effectors);

		void set_input_stream(std::shared_ptr< input::input_stream const > strm);

//		todo: get_available_bindings() const; list of named buttons/axis
		void bind_button(input::btn_element_id btn, unsigned long effector);
		void bind_axis(input::axis_element_id axis, unsigned long effector);
		void auto_bind_all();	// Auto bind a button to every effector

		binding_info_list get_current_bindings() const;

		virtual input_id_list_t get_input_ids() const override;
		virtual output_list_t process(input_list_t const&) override;

	protected:
		virtual double get_activation(unsigned long effector, input::input_stream_access const& input_data) const = 0;

	protected:
		std::shared_ptr< input::input_stream const > in_strm_;

		size_t num_effectors_;
		std::map< unsigned long, input::btn_element_id > bound_btns_;
		std::map< unsigned long, input::axis_element_id > bound_axes_;
	};

}


#endif

