// cmdline.h

#ifndef __WB_PARAMS_CMDLINE_H
#define __WB_PARAMS_CMDLINE_H

#include "schema.h"
#include "param_accessor.h"
#include "param_yaml.h"

#include <string>
#include <ostream>


namespace prm {

	class cmdline_processor
	{
	public:
		cmdline_processor(
			schema::schema_provider_map_handle provider,
			param_tree& pt
			);

	public:
		std::string prompt() const;
		bool process_cmd(std::string const& cmd, std::ostream& out);
		void enter_cmd_loop(std::istream& in, std::ostream& out, std::string const& quit_str = ":q");

	private:
		void reload_pt_schema();

	private:
		schema::schema_provider_map_handle m_provider;
		param_tree& m_pt;
		param_accessor m_accessor;

		friend class cmd_visitor;
	};
	
}



#endif


