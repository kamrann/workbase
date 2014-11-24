// passive_controller.h

#ifndef __NE_RTP_PASSIVE_CONRTOLLER_H
#define __NE_RTP_PASSIVE_CONRTOLLER_H

#include "controller_defn.h"
#include "controller.h"


namespace sys {

	class passive_controller_defn:
		public i_controller_defn
	{
	public:
		virtual std::string get_name() const override;
		virtual std::string update_schema_providor(prm::schema::schema_provider_map_handle provider, prm::qualified_path const& prefix) const override;
		virtual controller_ptr create_controller(prm::param_accessor acc) const override;
	};

	class passive_controller:
		public i_controller
	{
	public:
		virtual input_id_list_t get_input_ids() const override;
		virtual output_list_t process(input_list_t const& inputs) override;
	};

}


#endif

