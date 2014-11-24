// dynamic_enum_schema.h

#ifndef __WB_DYNAMIC_ENUM_SCHEMA_H
#define __WB_DYNAMIC_ENUM_SCHEMA_H

#include "param_accessor.h"
#include "schema_builder.h"

#include <string>
#include <functional>


namespace prm {

	namespace sb = schema;

	template <
		typename Result
//		, typename SchemaArgs,
		, typename... GenArgs
	>
	class dynamic_enum_schema
	{
	public:
		typedef Result result_t;

		typedef std::function< void(schema::schema_provider_map_handle, qualified_path const&) > schema_fn_t;
		typedef std::function< void(schema::schema_provider_map_handle, qualified_path const&, int) > external_schema_fn_t;
		typedef std::function< result_t(param_accessor, GenArgs...) > generator_fn_t;

	protected:
		struct option
		{
			schema_fn_t schema_fn;
			external_schema_fn_t ext_schema_fn;
			generator_fn_t gen_fn;
		};

	public:
		dynamic_enum_schema()
		{}

		void register_option(
			std::string name,
			schema_fn_t schema_fn,
			generator_fn_t gen_fn,
			external_schema_fn_t ext_schema_fn = {}
			)
		{
			m_options[name] = { schema_fn, ext_schema_fn, gen_fn };
		}

		template < typename T >
		void register_option(
			std::string name,
			std::shared_ptr< T > ftr
			)
		{
			option opt;
			opt.schema_fn = [ftr](schema::schema_provider_map_handle provider, qualified_path const& prefix)
			{
				ftr->update_schema_provider(provider, prefix);
			};
			opt.ext_schema_fn = [ftr](schema::schema_provider_map_handle provider, qualified_path const& prefix, int component)
			{
				ftr->update_schema_provider(provider, prefix, component);
			};
			opt.gen_fn = [ftr](param_accessor acc, GenArgs... args) -> result_t
			{
				return ftr->generate(acc, args...);
			};
			m_options[name] = opt;
		}

		void update_schema_provider(schema::schema_provider_map_handle provider, qualified_path const& prefix)// const
		{
			auto path = prefix;

			std::vector< std::string > enum_values;
			for(auto const& opt : m_options)
			{
				// Append option name to selection list
				auto opt_name = opt.first;
				enum_values.push_back(opt_name);

				// Register option schema
				auto const& opt_impl = opt.second;
				opt_impl.schema_fn(provider, path + opt_name);
			}

			// Selection box
			auto sel_name = std::string{ "type" };//m_name + "_type";
			(*provider)[path + sel_name] = [=](param_accessor acc)
			{
				auto s = sb::enum_selection(
					sel_name,
					enum_values,
					0,
					1
					);
				return s;
			};

			// Overall schema is selection box followed by content schema of selected option
			(*provider)[path] = [=](param_accessor acc)
			{
				// TODO: Maybe should just automate is_available() (+ perhaps accesses in child/descendent mode?)
				// to fail if the accessor is locked due to failed move attempt?
				auto valid = acc.move_to(path);

				auto s = sb::list(path.leaf().name());
				sb::append(s, provider->at(path + sel_name)(acc));

				auto imm = acc(param_accessor::LookupMode::Child);
				if(valid && imm.is_available(sel_name) && !is_unspecified(imm[sel_name]))
				{
					auto opt_name = extract_as< enum_param_val >(imm[sel_name])[0];
					sb::append(s, provider->at(path + opt_name)(acc));
				}
				return s;
			};

			m_core_path = path;
		}

		void update_schema_provider(schema::schema_provider_map_handle provider, qualified_path const& prefix, int component) const
		{
			auto path = prefix;

			for(auto const& opt : m_options)
			{
				// Register option component schema
				auto opt_name = opt.first;
				auto const& opt_impl = opt.second;
				opt_impl.ext_schema_fn(provider, path + opt_name, component);
			}

			(*provider)[path] = [=](param_accessor acc)
			{
				acc.move_to(path);

				auto s = sb::list(path.leaf().name());
				// Need to access from the position 
				auto sel_acc = acc(param_accessor::LookupMode::Child);
				sel_acc.move_to(m_core_path);
				auto sel_name = std::string{ "type" };
				if(sel_acc.is_available(sel_name) && !is_unspecified(sel_acc[sel_name]))
				{
					auto opt_name = extract_as< enum_param_val >(sel_acc[sel_name])[0];
					sb::append(s, provider->at(path + opt_name)(acc));
				}
				return s;
			};
		}

		result_t generate(param_accessor acc, GenArgs... args) const
		{
			//acc.move_to(m_core_path);
			auto sel_name = std::string{ "type" };//m_name + "_type";
			auto opt_name = extract_as< prm::enum_param_val >(acc(param_accessor::LookupMode::Child)[sel_name])[0];
			acc.move_relative(opt_name);
			return m_options.at(opt_name).gen_fn(acc, args...);
		}

	protected:
		//std::string m_name;
		std::map< std::string, option > m_options;
		qualified_path m_core_path;
	};

}



#endif


