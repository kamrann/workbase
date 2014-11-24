// ready_state.cpp

#include "ready_state.h"
#include "control_fsm.h"

#include "../ga_params.h"


namespace ga_control {
	namespace fsm {

		void ready::on_repeat(clsm::ev_cmd< repeat_cmd > const& cmd)
		{
			// TODO: command arguments
			size_t const num_trials = 10;
			size_t const num_generations = 1000;

			auto& ga_ctx = context< ga_controller >();
			auto acc = prm::param_accessor{ &ga_ctx.ptree };
			auto params_as_yaml = ga_ctx.ptree.convert_to_yaml();

			auto& defn = ga_ctx.ga_def;
			auto domain = defn.generate_domain(acc);
			auto alg = defn.generate_alg(acc, domain);

			for(size_t trial = 0; trial < num_trials; ++trial)
			{
				// TODO: TEMP - sort out seeding - see note in ga_controller fsm constructor
				alg->reset(static_cast<uint32_t>(std::chrono::high_resolution_clock::now().time_since_epoch().count() & 0xffffffff));
					//context< ga_controller >().ga_rseed);

				ga_ctx.serializer->serialize_start(params_as_yaml);

				for(size_t gen = 0; gen < num_generations; ++gen)
				{
					alg->epoch();

					auto diversity = domain->population_genetic_diversity(alg->population());
					ga_ctx.serializer->serialize_generation(alg->generation(), alg->population(), diversity);
				}

				ga_ctx.serializer->serialize_end();
			}
		}

		std::string ready::get_prompt() const
		{
			return "@ready>";
		}

	}
}



