// db_evolved_controller.cpp

#include "db_evolved_controller.h"
#include "../evolved_system_domain_defn.h"

#include "system_sim/controller.h"

#include "evo_database/evo_db.h"

//#include "params/param_accessor.h"
//#include "params/schema_builder.h"
#include "ddl/values/yaml_conversion.h"

#include <Wt/Dbo/Transaction>


namespace dbo = Wt::Dbo;

namespace sys {
	namespace ev {

		db_evolved_controller_defn::db_evolved_controller_defn(session_fn_t session_fn):
			session_fn_(session_fn)
		{

		}

		std::string db_evolved_controller_defn::get_name() const
		{
			return "db_evolved";
		}
		
		ddl::defn_node db_evolved_controller_defn::get_defn(ddl::specifier& spc)
		{
			auto enum_vals_fn = ddl::enum_defn_node::enum_values_str_fn_t{ [this](ddl::navigator nav)
			{
				// TODO: currently retrieving list from database here and storing in schema.
				// alternative would be to have schema support a database query select control,
				// and then just pass query string as its parameter.

				auto db_s = session_fn_();
				dbo::Transaction t(*db_s);
				auto query = db_s->find< named_genome >();
				auto results = query.resultList();

				ddl::enum_defn_node::enum_str_set_t entries;
				for(auto row : results)
				{
					entries.insert(std::end(entries), row->name);
				}
				t.commit();

				return entries;
			}
			};

			return spc.enumeration("db_controller")
				(ddl::define_enum_func{ enum_vals_fn })
				;
		}

		controller_ptr db_evolved_controller_defn::create_controller(ddl::navigator nav) const
		{
			// TODO: Need schema enum to support separate label and value strings, then use the 
			// value string to store database row id.
			auto name = nav.get().as_single_enum_str();

			auto db_s = session_fn_();
			dbo::Transaction t(*db_s);
			auto query = db_s->find< named_genome >().where("name = ?").bind(name);
			auto query_result = query.resultValue();

			auto pgn = query_result->gn;
			auto pevo_run = pgn->generation->run;
			auto run_params = YAML::Load(pevo_run->params);

			// TODO: This is not good
			run_params = run_params["composite"]["domain_cfg"]["composite"]["contents"]["conditional"];
			if(!run_params.IsDefined())
			{
				throw std::runtime_error("failed to find yaml node for evolved system");
			}

			/* TODO:
			This seems like overkill, to generate an entire domain/system when all we need is the decoding
			function of the genetic mapping. May be unavoidable though...
			*/

			ddl::specifier spc;
			evolved_system_domain_defn domain_defn;
			auto sch_defn = domain_defn.get_defn(spc);
			ddl::sd_tree tree;
			auto root_attribs = ddl::sd_node_attribs{};
			root_attribs.defn = sch_defn;
			tree.create_root(root_attribs);
			auto load_ok = ddl::import_yaml(run_params, tree, ddl::sd_node_ref{ tree.get_root() });
			if(!load_ok)
			{
				return nullptr;
			}

			auto ev_nav = ddl::navigator{ &tree, ddl::sd_node_ref{ tree.get_root() } };
			auto domain = domain_defn.generate(ev_nav);
			auto gn = domain->mapping_->load_genome_from_binary(pgn->encoding);
			auto controller = domain->mapping_->decode(gn);

			t.commit();

			return controller;
		}

	}
}



