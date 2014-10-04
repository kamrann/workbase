// basic_system_defn.h

#ifndef __NE_BASIC_SYSTEM_DEFN_H
#define __NE_BASIC_SYSTEM_DEFN_H

#include "system_defn.h"

#include <list>
#include <map>
#include <memory>


namespace sys {

	class basic_system_defn:
		public i_system_defn
	{
	public:
		virtual void add_controller_defn(std::string controller_class, std::unique_ptr< i_controller_defn > defn) override;
		
		// Must be called after all controller definitions have been registered
		std::string update_schema_provider_for_agent_controller(prm::schema::schema_provider_map_handle provider, prm::qualified_path const& prefix) const;

	protected:
		controller_ptr create_controller(prm::param_accessor acc) const;

	protected:
		typedef std::list< std::unique_ptr< i_controller_defn > > controller_defn_list_t;
		typedef std::map< std::string, std::shared_ptr< controller_defn_list_t > > controller_defn_map_t;

		controller_defn_map_t m_controller_defns;
	};

}


#endif

