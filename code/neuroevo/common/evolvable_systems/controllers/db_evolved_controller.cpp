// db_evolved_controller.cpp

#include "db_evolved_controller.h"
#include "../evolved_system_domain_defn.h"

#include "system_sim/controller.h"

#include "evo_database/evo_db.h"

#include "params/param_accessor.h"
#include "params/schema_builder.h"

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

		namespace sb = prm::schema;
		
		std::string db_evolved_controller_defn::update_schema_providor(prm::schema::schema_provider_map_handle provider, prm::qualified_path const& prefix) const
		{
			auto path = prefix;

			(*provider)[path + std::string{ "db_controller" }] = [=](prm::param_accessor acc)
			{
				// TODO: currently retrieving list from database here and storing in schema.
				// alternative would be to have schema support a database query select control,
				// and then just pass query string as its parameter.

				auto db_s = session_fn_();
				dbo::Transaction t(*db_s);
				auto query = db_s->find< named_genome >();
				auto results = query.resultList();

				std::vector< std::string > entries;
				for(auto row : results)
				{
					entries.emplace_back(row->name);
				}
				t.commit();

				auto s = sb::enum_selection("db_controller", entries, 0, 1);
				sb::unlabel(s);
				return s;
			};

			(*provider)[path] = [=](prm::param_accessor acc)
			{
				auto s = sb::list(path.leaf().name());
				sb::append(s, provider->at(path + std::string("db_controller"))(acc));
				return s;
			};

			return path.leaf().name();
		}

		controller_ptr db_evolved_controller_defn::create_controller(prm::param_accessor acc) const
		{
			// TODO: Need schema enum to support separate label and value strings, then use the 
			// value string to store database row id.
			auto name = prm::extract_as< prm::enum_param_val >(acc["db_controller"])[0];

			auto db_s = session_fn_();
			dbo::Transaction t(*db_s);
			auto query = db_s->find< named_genome >().where("name = ?").bind(name);
			auto query_result = query.resultValue();

			auto pgn = query_result->gn;
			auto pevo_run = pgn->generation->run;
			auto run_params = YAML::Load(pevo_run->params);

			prm::param_tree ev_ptree = prm::param_tree::generate_from_yaml(run_params);
			prm::param_accessor ev_acc(&ev_ptree);

			/* TODO:
			This seems like overkill, to generate an entire domain/system when all we need is the decoding
			function of the genetic mapping. May be unavoidable though...
			*/
			evolved_system_domain_defn domain_defn;
			auto domain = domain_defn.generate(ev_acc);
			auto gn = domain->mapping_->load_genome_from_binary(pgn->encoding);
			auto controller = domain->mapping_->decode(gn);

			t.commit();

			return controller;
		}

	}
}



