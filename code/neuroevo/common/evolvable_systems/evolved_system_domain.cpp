// evolved_system_domain.cpp

#include "evolved_system_domain.h"

#include "system_sim/system.h"
#include "system_sim/controller.h"


namespace sys {
	namespace ev {

		evolved_system_domain::evolved_system_domain(
			system_ptr sys,
			std::unique_ptr< i_genetic_mapping > mapping,
			std::unique_ptr< objective > objective
			):
			system_(std::move(sys)),
			mapping_(std::move(mapping)),
			objective_(std::move(objective))
		{

		}

		ga::objective_value::type evolved_system_domain::objective_value_type() const
		{
			return objective_->get_type();
		}

		ga::genome evolved_system_domain::create_random_genome(ga::rgen_t& rgen) const
		{
			return mapping_->create_random_genome(rgen);
		}

		ga::crossover_fn_t evolved_system_domain::crossover_op() const
		{
			return mapping_->crossover_op();
		}

		ga::mutation evolved_system_domain::mutation_op() const
		{
			return mapping_->mutation_op();
		}

		bool evolved_system_domain::rectify_genome(ga::genome& gn) const
		{
			return mapping_->rectify_genome(gn);
		}

		std::vector< ga::objective_value > evolved_system_domain::evaluate_genome(ga::genome const& gn, ga::rgen_t& rgen, size_t trials) const
		{
			auto controller = mapping_->decode(gn);

			/* TODO: !!!!!!!!!! temp hack */
			system_->register_agent_controller(0, std::move(controller));

			std::vector< ga::objective_value > results(trials, ga::objective_value{});
			for(size_t t = 0; t < trials; ++t)
			{
				// TODO: Not sure how best to deal with this. Perhaps ideally domains do not have their own rgens
				// (as currently i_system does) but are passed a reference to an external rgen.
				auto seeder = std::uniform_int_distribution < unsigned long > {};
				system_->set_random_seed(seeder(rgen));
				system_->initialize();
				// rand seed? 

				objective_->reset();

				bool still_going = true;
				while(still_going)
				{
					still_going = system_->update();
					objective_->update();
				}

				results[t] = objective_->evaluate();
			}
			return results;
		}

		ga::diversity_t evolved_system_domain::population_genetic_diversity(ga::genetic_population const& pop) const
		{
			return mapping_->population_genetic_diversity(pop);
		}

		void evolved_system_domain::output_individual(ga::genome const& gn, std::ostream& os) const
		{
			mapping_->output_individual(gn, os);
		}

	}
}




