// ackley.cpp

#include "ackley.h"


namespace ga {
	namespace domain {
		namespace fnopt {
			namespace functions {

				ddl::defn_node ackley_schema::get_defn(ddl::specifier& spc)
				{
					using ddl::defn_node;

					defn_node dims = spc.integer("dims")
						(ddl::spc_min < ddl::integer_defn_node::value_t > { 1 })
						(ddl::spc_default< ddl::integer_defn_node::value_t >{ 2 })
						;
					defn_node lower = spc.realnum("lower")
						(ddl::spc_default< ddl::realnum_defn_node::value_t >{ -40.0 })
						;
					defn_node upper = spc.realnum("upper")
						(ddl::spc_default< ddl::realnum_defn_node::value_t >{ 40.0 })
						;
					// TODO: Should these constants be bounded?
					defn_node a = spc.realnum("a")
						(ddl::spc_default< ddl::realnum_defn_node::value_t >{ 20.0 })
						;
					defn_node b = spc.realnum("b")
						(ddl::spc_default< ddl::realnum_defn_node::value_t >{ 0.2 })
						;
					defn_node c_pi = spc.realnum("c_pi")
						(ddl::spc_default< ddl::realnum_defn_node::value_t >{ 2.0 })
						;

					return spc.composite("ackley")(ddl::define_children{}
						("dimensions", dims)
						("lower_bound", lower)
						("upper_bound", upper)
						("a", a)
						("b", b)
						("c_pi", c_pi)
						);
				}

				function_opt_domain::function_defn ackley_schema::generate(ddl::navigator nav) const
				{
					auto dim = nav["dimensions"].get().as_int();
					auto lower = nav["lower_bound"].get().as_real();
					auto upper = nav["upper_bound"].get().as_real();
					auto a = nav["a"].get().as_real();
					auto b = nav["b"].get().as_real();
					auto c = nav["c_pi"].get().as_real() * M_PI;

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


