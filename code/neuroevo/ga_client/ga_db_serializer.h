// ga_db_serializer.h

#ifndef __WB_GA_DATABASE_SERIALIZER_H
#define __WB_GA_DATABASE_SERIALIZER_H

#include "ga_serializer.h"

#include "genetic_algorithm/genetic_population.h"

#include "evo_database/evo_db.h"


namespace ga {

	class ga_db_serializer:
		public i_ga_serializer
	{
	public:
		ga_db_serializer(evodb_session& dbs);

	public:
		virtual void serialize_start(YAML::Node params) override;
		virtual void serialize_pause() override;
		virtual void serialize_end() override;
		virtual void serialize_generation(size_t gen_index, genetic_population const& pop, double diversity) override;
		/*virtual */dbo::ptr< ::genome > serialize_genome(genetic_population::individual const& idv, int rank, dbo::ptr< generation > gen_ptr);// override;

		virtual bool store_named(size_t idv_idx, std::string const& name) override;

	protected:
		void persist_all();

	protected:
		static const size_t GenerationBatchSize = 100;	// TODO:
		static const size_t GenomeBatchSize = 1000;	// TODO:

		evodb_session& _dbs;
		dbo::ptr< evo_run > _run_ptr;
		std::vector< dbo::ptr< generation > > _generations;
		std::vector< dbo::ptr< ::genome > > _genomes;

//		size_t _last_added_gen;
		std::vector< dbo::ptr< ::genome > > _cur_gen_pop;
	};

}


#endif


