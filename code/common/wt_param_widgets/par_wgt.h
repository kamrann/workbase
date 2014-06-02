// par_wgt.h

#ifndef __WB_PARAM_WIDGET_H
#define __WB_PARAM_WIDGET_H

#include "pw_yaml.h"

#include "params/param.h"
#include "params/param_constraints.h"

#include <Wt/WCompositeWidget>


namespace prm
{
	typedef par_constraints pw_options;

	class param_wgt:
		public Wt::WCompositeWidget
	{
	public:
		param_wgt(): m_base_impl(nullptr)
		{}
//		void initialize(pw_options const& opt, std::string const& label);
		void initialize(YAML::Node const& script);

	public:
//		static param_wgt* create(ParamType type, pw_options const& opt, std::string const& id, std::string const& label = "");

		// Create a leaf widget (excludes container widget, script passed in must be YAML map component of a schema, where 'type' is not 'container')
		static param_wgt* create(YAML::Node const& script);

		// Create widget tree from schema
		typedef std::function< YAML::Node(YAML::Node const&) > schema_provider_fn;
		static param_wgt* create(schema_provider_fn schema_provider);

	public:
		virtual void connect_changed_handler(boost::function< void() > const& handler);
		inline std::string const& get_id() const
		{
			return m_id;
		}
		virtual param get_param() const = 0;
		virtual YAML::Node get_yaml_param() const;
		std::string get_yaml_script() const
		{
			return YAML::Dump(get_yaml_param());
		}
		virtual void set_from_param(param const& p) = 0;

	protected:
//		virtual Wt::WWidget* create_impl(pw_options const& opt) = 0;
		virtual Wt::WWidget* create_impl(YAML::Node const& script) = 0;

	protected:
		Wt::WWidget* m_base_impl;
		std::string m_id;	// TODO: Or store complete schema component?
	};
}


#endif


