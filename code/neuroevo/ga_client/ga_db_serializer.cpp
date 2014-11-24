// ga_db_serializer.cpp

#include "ga_db_serializer.h"

#include "genetic_algorithm/genome.h"
#include "genetic_algorithm/genetic_population.h"


namespace ga {

	ga_db_serializer::ga_db_serializer(evodb_session& dbs):
		_dbs(dbs)
	{
		// NOTE: Making assumption that vector implementation will never alter this space allocation on
		// calls to clear()
		_generations.reserve(GenerationBatchSize);
		_genomes.reserve(GenomeBatchSize);

//		_last_added_gen = 0;
	}

	void ga_db_serializer::serialize_start(YAML::Node params)
	{
		// Add database entry to evo_run table
		dbo::Transaction t(_dbs);
		_run_ptr = _dbs.add(new evo_run);
		_run_ptr.modify()->params = YAML::Dump(params);
		_run_ptr.modify()->started = std::chrono::system_clock::now();
		t.commit();

		_generations.clear();
	}

	void ga_db_serializer::serialize_pause()
	{
		persist_all();
	}
	
	void ga_db_serializer::serialize_end()
	{
		dbo::Transaction t(_dbs);
		persist_all();
		if(_run_ptr->generations.size() > 0)
		{
			_run_ptr.modify()->ended = std::chrono::system_clock::now();
		}
		else
		{
			_run_ptr.remove();
		}
		t.commit();
	}
	
	void ga_db_serializer::serialize_generation(size_t gen_index, genetic_population const& pop, double diversity)
	{
//		_last_added_gen = gen_index;
		_cur_gen_pop.clear();
		_cur_gen_pop.resize(pop.size(), dbo::ptr < ::genome > {});

		auto gen = new generation;
		gen->index = gen_index;
		gen->highest_obj_val = YAML::Dump(pop[0].obj_val.to_yaml());	// Assuming fitness-sorted population
		gen->genetic_diversity = diversity;
		gen->run = _run_ptr;
		auto gen_ptr = dbo::ptr< generation >(gen);

		_generations.push_back(gen_ptr);

		if(_generations.size() == GenerationBatchSize)
		{
			persist_all();
		}

		// TODO: This should really be dealt with in interface base class...
		switch(_generation_gn_opt)
		{
			case Generation::StoreAll:
			{
				size_t idx = 0;
				for(auto const& idv : pop)
				{
					_cur_gen_pop[idx++] = serialize_genome(idv, 0 /* TODO: obj val rank */, gen_ptr);
				}
			}
			break;

			case Generation::StoreBest:
			{
				auto highest_fitness = pop[0].fitness;
				for(size_t i = 0; i < pop.size() && pop[i].fitness == highest_fitness; ++i)
				{
					_cur_gen_pop[i] = serialize_genome(pop[i], 0, gen_ptr);
				}
			}
			break;
		}
	}

	dbo::ptr< ::genome > ga_db_serializer::serialize_genome(genetic_population::individual const& idv, int rank, dbo::ptr< generation > gen_ptr)
	{
		auto gn_rec = new ::genome;
		// TODO: objective_value to_yaml()
		gn_rec->objective_value = YAML::Dump(idv.obj_val.to_yaml());
		gn_rec->gen_obj_rank = rank;
		gn_rec->encoding = idv.gn.to_binary();
		gn_rec->generation = gen_ptr;
		auto gn_ptr = dbo::ptr< ::genome >(gn_rec);

		_genomes.push_back(gn_ptr);

		if(_genomes.size() == GenomeBatchSize)
		{
			persist_all();
		}

		return gn_ptr;
	}

	void ga_db_serializer::persist_all()
	{
		dbo::Transaction t(_dbs);
		
		for(auto& g : _generations)
		{
			_dbs.add(g);
		}
		_generations.clear();

		for(auto& gn : _genomes)
		{
			_dbs.add(gn);

/*			if(gn->generation->index == _last_added_gen)
			{
				_cur_gen_pop.push_back(gn);
			}
*/		}
		_genomes.clear();

		t.commit();
	}

	bool ga_db_serializer::store_named(size_t idv_idx, std::string const& name)
	{
		auto ptr = _cur_gen_pop[idv_idx];
		if(ptr)
		{
			// TODO: check if already named?
			auto named = new ::named_genome{};
			named->gn = ptr;
			named->name = name;
			dbo::Transaction t(_dbs);
			_dbs.add(named);
			t.commit();
			return true;
		}
		else
		{
			// Not serialized
			return false;
		}
	}

}



