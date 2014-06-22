// par_wgt.h

#ifndef __WB_PARAM_WIDGET_H
#define __WB_PARAM_WIDGET_H

#include "params/param.h"
#include "params/param_constraints.h"

#include <Wt/WCompositeWidget>


namespace YAML {
	class Node;
}

namespace prm
{
	typedef std::function< YAML::Node(std::string const&) > schema_accessor_fn;

	class param_tree: public Wt::WCompositeWidget
	{
	public:
		typedef std::function< YAML::Node(YAML::Node const&) > schema_provider_fn;

		static param_tree* create(schema_provider_fn schema_provider);

	public:
		YAML::Node get_yaml_param();// const;
		void set_from_yaml_param(YAML::Node const& vals);

	public:
		class param_wgt:
			public Wt::WCompositeWidget
		{
		public:
			param_wgt(): m_base_impl(nullptr)
			{}

			void initialize(YAML::Node const& script);

		public:
			// Create widget tree from schema
			typedef std::function< YAML::Node(YAML::Node const&) > schema_provider_fn;

		public:
			virtual void connect_changed_handler(std::function< void() > const& handler);
			inline std::string const& get_id() const { return m_id; }
			virtual param get_param() const = 0;
			virtual YAML::Node get_yaml_param() const;
			std::string get_yaml_script() const;
			virtual void set_from_param(param const& p) = 0;
			void set_from_yaml_param(YAML::Node const& p);

		protected:
			static param_wgt* create(YAML::Node const& schema, schema_accessor_fn& schema_accessor);

			virtual Wt::WWidget* create_impl(YAML::Node const& script) = 0;

		protected:
			Wt::WWidget* m_base_impl;
			std::string m_id;	// TODO: Or store complete schema component?

		friend class param_tree;
		};

	private:
		param_tree();

	private:
		schema_accessor_fn m_schema_accessor;
	};
	
}


#endif


