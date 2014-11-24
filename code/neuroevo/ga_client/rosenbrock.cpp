// rosenbrock.cpp

#include "rosenbrock.h"

#include "params/param_accessor.h"
#include "params/schema_builder.h"


namespace ga {
	namespace domain {
		namespace fnopt {
			namespace functions {

				namespace sb = prm::schema;

				void rosenbrock_defn::update_schema_provider(prm::schema::schema_provider_map_handle provider, prm::qualified_path const& prefix) const
				{
					auto path = prefix;

					(*provider)[path + std::string{ "dimensions" }] = [=](prm::param_accessor acc)
					{
						return sb::integer("dimensions", 2, 1);
					};
					(*provider)[path + std::string{ "param_lower_bound" }] = [=](prm::param_accessor acc)
					{
						return sb::real("param_lower_bound", -50.0);
					};
					(*provider)[path + std::string{ "param_upper_bound" }] = [=](prm::param_accessor acc)
					{
						return sb::real("param_upper_bound", 50.0);
					};
					(*provider)[path + std::string{ "a" }] = [=](prm::param_accessor acc)
					{
						return sb::real("a", 1.0);
					};
					(*provider)[path + std::string{ "b" }] = [=](prm::param_accessor acc)
					{
						return sb::real("b", 100.0);
					};

					(*provider)[path] = [=](prm::param_accessor acc)
					{
						auto s = sb::list(path.leaf().name());
						sb::append(s, provider->at(path + std::string{ "dimensions" })(acc));
						sb::append(s, provider->at(path + std::string{ "param_lower_bound" })(acc));
						sb::append(s, provider->at(path + std::string{ "param_upper_bound" })(acc));
						sb::append(s, provider->at(path + std::string{ "a" })(acc));
						sb::append(s, provider->at(path + std::string{ "b" })(acc));
						return s;
					};
				}

				function_opt_domain::function_defn rosenbrock_defn::generate(prm::param_accessor acc) const
				{
					auto dim = prm::extract_as< int >(acc["dimensions"]);
					auto lower = prm::extract_as< double >(acc["param_lower_bound"]);
					auto upper = prm::extract_as< double >(acc["param_upper_bound"]);
					auto a = prm::extract_as< double >(acc["a"]);
					auto b = prm::extract_as< double >(acc["b"]);

					return function_opt_domain::function_defn(rosenbrock(
						dim,
						lower,
						upper,
						a,
						b
						));
				}


				function_opt_domain::function_defn rosenbrock(
					size_t n_dimensions,
					double lower,
					double upper,
					double a,
					double b
					)
				{
					function_opt_domain::function_defn defn;
					defn.parameters.resize(n_dimensions);
					for(auto& pd : defn.parameters)
					{
						pd.lower_bound = lower;
						pd.upper_bound = upper;
					}

					defn.eval = [n_dimensions, a, b](function_opt_domain::parameter_list const& p)
					{
						double sum = 0.0;
						for(size_t i = 0; i < n_dimensions - 1; ++i)
						{
							auto temp1 = p[i + 1] - p[i] * p[i];
							auto temp2 = p[i] - a;
							sum += (b * temp1 * temp1) + temp2 * temp2;
						}

						return sum;
					};

					return defn;
				}

			}
		}
	}
}


