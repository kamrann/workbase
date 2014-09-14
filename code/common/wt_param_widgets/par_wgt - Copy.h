// par_wgt.h

#ifndef __WB_PARAM_WIDGET_H
#define __WB_PARAM_WIDGET_H

#include "params/param.h"
#include "params/param_constraints.h"
#include "qualified_path.h"
#include "pw_schema.h"

#include "pw_exceptions.h"	// For convenience of includer

#include <Wt/WCompositeWidget>

#include <boost/optional.hpp>

#include <set>

/*
TODO: Seems like boost::optional< param > requires full definition...

namespace YAML {
	class Node;
}
*/
#include <yaml-cpp/yaml.h>


namespace prm
{
	typedef std::function< YAML::Node(qualified_path const&, param_accessor) > schema_accessor_fn;

	class param_accessor;
	class par_wgt_holder;

	class param_tree: public Wt::WCompositeWidget
	{
	public:
		class param_wgt;

		static param_tree* create(schema::schema_provider schema_provider);

	public:
		param get_yaml_param(bool instantiated);// const;
		void set_from_yaml_param(param const& vals); // Note that the passed param may contain import tags
		schema::schema_node get_schema(qualified_path const& schema_name);// , boost::optional< param > param_vals = boost::none);
		param_accessor get_param_accessor(qualified_path const& wrt = qualified_path());

		param_wgt* create_widget(
			schema::schema_node const& schema,
			qualified_path const& qual_path);
		void unregister_widget(param_wgt* wgt);
		bool register_changed_handler(
			qualified_path const& trigger_id,
			qualified_path const& recipient_id,
			std::function< void() > handler);
		void update_widget_from_schema(
			qualified_path const& wgt_id);

	public:
		class param_wgt:
			public Wt::WCompositeWidget
		{
		public:
			param_wgt();
			~param_wgt();

			void initialize(schema::schema_node const& script, qualified_path const& qual_path, param_tree* tree);

		public:
			// Create widget tree from schema
			typedef std::function< YAML::Node(YAML::Node const&) > schema_provider_fn;

		public:
			virtual Wt::Signals::connection connect_changed_handler(std::function< void() > const& handler);
			inline qualified_path const& get_id() const { return m_id; }
			virtual param get_yaml_param(bool instantiated) const;
			virtual param get_locally_instantiated_yaml_param(bool recursively_instantiate) const = 0;
			std::string get_yaml_script(bool instantiated) const;
			virtual void set_from_yaml_param(param const& p);// = 0; // Note that the passed param may contain import tags
			virtual void update_impl_from_yaml_param(param const& p) = 0;

		protected:
			static param_wgt* create(
				schema::schema_node const& schema,
				qualified_path const& qual_path,
				param_tree* tree);

			virtual Wt::WWidget* create_impl(schema::schema_node const& script) = 0;

		protected:
			enum class MenuItemId {
				SaveAs,
				Modify,
			};

			std::string update_status_text();

			void on_save_as();
			void on_modify();

		protected:
			Wt::WContainerWidget* m_container;
			par_wgt_holder* m_holder;
			Wt::WWidget* m_base_impl;
			qualified_path m_id;	// TODO: Or store complete schema component?
			std::string m_filter;
			boost::optional< std::string > m_import;
			Wt::WPopupMenu* m_menu;
			std::map< MenuItemId, Wt::WMenuItem* > m_menu_items;
			//Wt::WText* m_status_text;
			param_tree* m_tree;

		friend class param_tree;
		};

	private:
		param_tree();

	public:
		~param_tree();

	public:
		qualified_path resolve_id(std::string const& id, boost::optional< qualified_path > const& parallel = boost::none) const;
		param_wgt* find_widget(qualified_path const& path) const;

	private:
		void trigger_event(std::string const& event_name, qualified_path const& trigger_path);

	private:
		schema_accessor_fn m_schema_accessor;
		std::map< qualified_path, param_wgt* > m_widgets;
		std::map< qualified_path, std::set< Wt::Signals::connection > > m_connections;	// key references widgets on the
			// receiving end of a connection, not the widget whose change triggers the signal.

		typedef bool listener_data_t;	// TODO: more listening options
		std::map< std::string, std::map< qualified_path, listener_data_t > > m_listeners;	// For every event name, a list of widget ids that need to be updated.

		boost::optional< param > m_extended_params;
	};
	
}


#endif


