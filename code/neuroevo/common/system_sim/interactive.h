// interactive.h

#ifndef __NE_RTP_PHYS_INTERACTIVE_CONRTOLLER_H
#define __NE_RTP_PHYS_INTERACTIVE_CONRTOLLER_H

#include "interactive_controller.h"
#include "interactive_input.h"

//#include "params/param.h"

#include <boost/optional.hpp>

#include <vector>
#include <map>
#include <memory>


namespace sys
{
	class interactive_controller:
		public i_interactive_controller
	{
	public:
// TODO:		static boost::optional< unsigned long > extract_input_binding(prm::param binding);
//		static std::unique_ptr< interactive_controller > create_instance(prm::param_accessor param_vals);

	public:
		interactive_controller(size_t num_effectors);

		void add_binding(unsigned long input, unsigned long effector);

		virtual input_id_list_t get_input_ids() const override;
		virtual output_list_t process(input_list_t const&) override;

		virtual const interactive_input_set& get_required_interactive_inputs() const override;
		virtual void register_input(interactive_input const& input) override;

	protected:
		interactive_input_set m_required_inputs;
		std::map< unsigned long, std::vector< size_t > > m_bindings;
		std::vector< unsigned long > m_activations;	// Note that integer is just to deal with possibility of
			// multiple keys mapping to a single effector. The activation state is still only on/off.
	};

}


#endif

