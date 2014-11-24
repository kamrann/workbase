// ackley.cpp

#include "ackley.h"

#include "params/param_accessor.h"
#include "params/schema_builder.h"


namespace ga {
	namespace domain {
		namespace fnopt {
			namespace functions {

				ddl::defn_node ackley_schema::get_defn(ddl::specifier& spc)
				{
					using ddl::defn_node;

					defn_node dims = spc.integer()
						(ddl::spc_min < ddl::integer_defn_node::value_t > { 1 })
						(ddl::spc_default< ddl::integer_defn_node::value_t >{ 2 })
						;
					defn_node lower = spc.realnum()
						(ddl::spc_default< ddl::realnum_defn_node::value_t >{ -40.0 })
						;
					defn_node upper = spc.realnum()
						(ddl::spc_default< ddl::realnum_defn_node::value_t >{ 40.0 })
						;
					// TODO: Should these constants be bounded?
					defn_node a = spc.realnum()
						(ddl::spc_default< ddl::realnum_defn_node::value_t >{ 20.0 })
						;
					defn_node b = spc.realnum()
						(ddl::spc_default< ddl::realnum_defn_node::value_t >{ 0.2 })
						;
					defn_node c_pi = spc.realnum()
						(ddl::spc_default< ddl::realnum_defn_node::value_t >{ 2.0 })
						;

					return spc.composite()(ddl::define_children{}
						("dimensions", dims)
						("lower_bound", lower)
						("upper_bound", upper)
						("a", a)
						("b", b)
						("c(pi)", c_pi)
						);
				}

				function_opt_domain::function_defn ackley_schema::generate(ddl::navigator nav) const
				{
					auto dim = nav["dimensions"].get().as_int();
					auto lower = nav["lower_bound"].as_real();
					auto upper = nav["upper_bound"].as_real();
					auto a = nav["a"].as_real();
					auto b = nav["b"].as_real();
					auto c = nav["c(pi)"].as_real() * M_PI;

					return function_opt_domain::function_defn(ackley(
						dim,
						lower,
						upper,
						a,
						b,
						c
						));
				}


				namespace sb = prm::schema;

				void ackley_defn::update_schema_provider(prm::schema::schema_provider_map_handle provider, prm::qualified_path const& prefix) const
				{
					auto path = prefix;

					(*provider)[path + std::string{ "dimensions" }] = [=](prm::param_accessor acc)
					{
						return sb::integer("dimensions", 2, 1);
					};
					(*provider)[path + std::string{ "param_lower_bound" }] = [=](prm::param_accessor acc)
					{
						return sb::real("param_lower_bound", -40.0);
					};
					(*provider)[path + std::string{ "param_upper_bound" }] = [=](prm::param_accessor acc)
					{
						return sb::real("param_upper_bound", 40.0);
					};
					(*provider)[path + std::string{ "a" }] = [=](prm::param_accessor acc)
					{
						return sb::real("a", 20.0);
					};
					(*provider)[path + std::string{ "b" }] = [=](prm::param_accessor acc)
					{
						return sb::real("b", 0.2);
					};
					(*provider)[path + std::string{ "c_pi" }] = [=](prm::param_accessor acc)
					{
						return sb::real("c_pi", 2.0);
					};

					(*provider)[path] = [=](prm::param_accessor acc)
					{
						auto s = sb::list(path.leaf().name());
						sb::append(s, provider->at(path + std::string{ "dimensions" })(acc));
						sb::append(s, provider->at(path + std::string{ "param_lower_bound" })(acc));
						sb::append(s, provider->at(path + std::string{ "param_upper_bound" })(acc));
						sb::append(s, provider->at(path + std::string{ "a" })(acc));
						sb::append(s, provider->at(path + std::string{ "b" })(acc));
						sb::append(s, provider->at(path + std::string{ "c_pi" })(acc));
						return s;
					};
				}

				function_opt_domain::function_defn ackley_defn::generate(prm::param_accessor acc) const
				{
					auto dim = prm::extract_as< int >(acc["dimensions"]);
					auto lower = prm::extract_as< double >(acc["param_lower_bound"]);
					auto upper = prm::extract_as< double >(acc["param_upper_bound"]);
					auto a = prm::extract_as< double >(acc["a"]);
					auto b = prm::extract_as< double >(acc["b"]);
					auto c = prm::extract_as< double >(acc["c_pi"]) * M_PI;

					return function_opt_domain::function_defn(ackley(
						dim,
						lower,
						upper,
						a,
						b,
						c
						));
				}


				function_opt_domain::function_defn ackley(
					size_t n_dimensions,
					double lower,
					double upper,
					double a,
					double b,
					double c
					)
				{
					function_opt_domain::function_defn defn;
					defn.parameters.resize(n_dimensions);
					for(auto& pd : defn.parameters)
					{
						pd.lower_bound = lower;
						pd.upper_bound = upper;
					}

					defn.eval = [n_dimensions, a, b, c](function_opt_domain::parameter_list const& p)
					{
						double sum_x_2 = 0.0;
						double sum_cos_cx = 0.0;
						for(auto const& x : p)
						{
							sum_x_2 += x * x;
							sum_cos_cx += std::cos(c * x);
						}

						return -a * std::exp(-b * std::sqrt(sum_x_2 / n_dimensions))
							- std::exp(sum_cos_cx / n_dimensions)
							+ a
							+ M_E;
					};

					return defn;
				}

			}
		}
	}
}


