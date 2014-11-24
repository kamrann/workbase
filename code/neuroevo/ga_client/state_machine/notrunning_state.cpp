// notrunning_state.cpp

#include "notrunning_state.h"
#include "control_fsm.h"


namespace ga_control {
	namespace fsm {

		not_running::not_running(my_context ctx): base(ctx)
		{
			reg_cmd< pop_cmd >(wrap_grammar< pop_cmd_parser< clsm::iter_t > >());
			reg_cmd< best_cmd >(wrap_grammar< best_cmd_parser< clsm::iter_t > >());
			reg_cmd< store_cmd >(wrap_grammar< store_cmd_parser< clsm::iter_t > >());

			// TODO: need to decide how to deal with initial population (ideally it shouldn't be procreated
			// before being evaluated), and what commands are available when paused at generation 0
			// - eg. if nothing has been saved to db, then can't call store command.
			context< ga_controller >().serializer->serialize_pause();
		}

		void not_running::on_pop(clsm::ev_cmd< pop_cmd > const& cmd)
		{
			std::stringstream ss;
			ss << "Population @ generation " << context< active >().alg->generation() << ":";
			context< ga_controller >().output_sink(ss.str());

			auto const& pop = context< active >().alg->population();
			for(auto const& idv : pop)
			{
				std::stringstream ss;
				std::fixed(ss);
				ss.precision(2);
				ss << "(" << idv.obj_val.to_string() << ") ";
				context< active >().domain->output_individual(idv.gn, ss);
				context< ga_controller >().output_sink(ss.str());
			}
		}

		void not_running::on_best(clsm::ev_cmd< best_cmd > const& cmd)
		{
			double highest = -std::numeric_limits< double >::max();
			std::vector< ga::genetic_population::individual const* > best;
			auto const& pop = context< active >().alg->population();
			for(auto const& idv : pop)
			{
				if(idv.fitness >= highest)
				{
					if(idv.fitness > highest)
					{
						highest = idv.fitness;
						best.clear();
					}
					best.push_back(&idv);
				}
			}

			std::stringstream ss;
			ss << "Fittest individual(s) @ generation " << context< active >().alg->generation() << ":";
			context< ga_controller >().output_sink(ss.str());

			for(auto const& idv : best)
			{
				ss.str("");
				std::fixed(ss);
				ss.precision(2);
				ss << "(" << idv->obj_val.to_string() << ") ";
				context< active >().domain->output_individual(idv->gn, ss);
				context< ga_controller >().output_sink(ss.str());
			}
		}

		void not_running::on_store(clsm::ev_cmd< store_cmd > const& cmd)
		{
			// TODO: Need to check whether or not the specified individual has already been serialized or not!
			// Currently just assuming it has

			if(cmd.name)
			{
				auto& serializer = context< ga_controller >().serializer;
				serializer->store_named(cmd.index, *cmd.name);
			}
		}

	}
}



