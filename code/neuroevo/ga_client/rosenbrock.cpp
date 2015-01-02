// rosenbrock.cpp

#include "rosenbrock.h"


namespace ga {
	namespace domain {
		namespace fnopt {
			namespace functions {

				ddl::defn_node rosenbrock_schema::get_defn(ddl::specifier& spc)
				{
					using ddl::defn_node;

					defn_node dims = spc.integer("dims")
						(ddl::spc_min < ddl::integer_defn_node::value_t > { 1 })
						(ddl::spc_default< ddl::integer_defn_node::value_t >{ 2 })
						;
					defn_node lower = spc.realnum("lower")
						(ddl::spc_default< ddl::realnum_defn_node::value_t >{ -50.0 })
						;
					defn_node upper = spc.realnum("upper")
						(ddl::spc_default< ddl::realnum_defn_node::value_t >{ 50.0 })
						;
					// TODO: Should these constants be bounded?
					defn_node a = spc.realnum("a")
						(ddl::spc_default< ddl::realnum_defn_node::value_t >{ 1.0 })
						;
					defn_node b = spc.realnum("b")
						(ddl::spc_default< ddl::realnum_defn_node::value_t >{ 100.0 })
						;

					return spc.composite("ackley")(ddl::define_children{}
						("dimensions", dims)
						("lower_bound", lower)
						("upper_bound", upper)
						("a", a)
						("b", b)
						);
				}

				function_opt_domain::function_defn rosenbrock_schema::generate(ddl::navigator nav) const
				{
					auto dim = nav["dimensions"].get().as_int();
					auto lower = nav["lower_bound"].get().as_real();
					auto upper = nav["upper_bound"].get().as_real();
					auto a = nav["a"].get().as_real();
					auto b = nav["b"].get().as_real();

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


