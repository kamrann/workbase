// ga_serializer.h

#ifndef __WB_GA_SERIALIZER_H
#define __WB_GA_SERIALIZER_H

#include <yaml-cpp/yaml.h>


namespace ga {

	class genetic_population;

	class i_ga_serializer
	{
	public:
		enum class Generation {
			StoreNone,
			StoreBest,
			StoreAll,
		};

	public:
		i_ga_serializer()
		{
			_generation_gn_opt = Generation::StoreNone;
		}

		void set_options(Generation opt)
		{
			_generation_gn_opt = opt;
		}

		virtual void serialize_start(YAML::Node params) = 0;
		virtual void serialize_pause() = 0;
		virtual void serialize_end() = 0;
		virtual void serialize_generation(size_t gen_index, ga::genetic_population const& pop, double diversity) = 0;
//		virtual void serialize_genome(ga::genome const& gn, int rank) = 0;

		virtual bool store_named(size_t idv_idx, std::string const& name) = 0;

	public:
		virtual ~i_ga_serializer()
		{}

	protected:
		Generation _generation_gn_opt;
	};

}


#endif


