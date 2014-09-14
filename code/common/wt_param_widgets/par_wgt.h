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

	struct pw_event
	{
		typedef std::string type;

		// TODO: Data needed?
	};

	typedef std::function< void() > pw_event_handler;

	class param_tree: public Wt::WCompositeWidget
	{
	public:
		class param_wgt;

		static param_tree* create(schema::schema_provider schema_provider);

	public:
		param get_yaml_param(bool instantiated);// const;
		bool set_from_yaml_param(param const& vals); // Note that the passed param may contain import tags
		schema::schema_node get_schema(qualified_path const& schema_name);// , boost::optional< param > param_vals = boost::none);
		param_accessor get_param_accessor(qualified_path const& wrt = qualified_path());

		param_wgt* create_widget(
			schema::schema_node const& schema,
			qualified_path const& qual_path);
		void unregister_widget(param_wgt* wgt);
		bool register_handler(
			qualified_path const& trigger_id,
			qualified_path const& recipient_id,
			pw_event::type type,
			pw_event_handler const& handler);
		void update_widget_from_schema(
			qualified_path const& wgt_id);

	public:
		class param_wgt_impl
		{
		public:
			enum {
				Default				= 0,

				ReadOnly			= 1 << 0,
			};

			typedef uint32_t options_t;

		public:
			virtual Wt::WWidget* get_wt_widget() = 0;
			virtual Wt::Signals::connection connect_handler(pw_event::type type, pw_event_handler const& handler) = 0;

			virtual param get_locally_instantiated_yaml_param(bool recursively_instantiate) const = 0;
			virtual bool update_impl_from_yaml_param(param const& p) = 0;

		public:
			virtual ~param_wgt_impl() {}
		};

		class param_wgt:
			public Wt::WCompositeWidget
		{
		public:
			param_wgt(qualified_path const& path, param_tree* tree);
			~param_wgt();

			void reset(bool clear_import);
			void initialize(schema::schema_node /*const&*/ script, bool clear_import);

		public:
			// Create widget tree from schema
			typedef std::function< YAML::Node(YAML::Node const&) > schema_provider_fn;

		public:
			virtual Wt::Signals::connection connect_handler(pw_event::type type, pw_event_handler const& handler);
			inline qualified_path const& get_id() const { return m_id; }
			virtual param get_yaml_param(bool instantiated) const;
			//virtual param get_locally_instantiated_yaml_param(bool recursively_instantiate) const = 0;
			std::string get_yaml_script(bool instantiated) const;
			virtual bool set_from_yaml_param(param const& p, boost::optional< std::string > import = boost::none);	// Note that the passed param may contain import tags
			//virtual void update_impl_from_yaml_param(param const& p) = 0;

		protected:
			static param_wgt* create(
				schema::schema_node const& schema,
				qualified_path const& qual_path,
				param_tree* tree);

			//virtual Wt::WWidget* create_impl(schema::schema_node const& script) = 0;

		protected:
			enum class MenuItemId {
				SaveAs,
				Modify,
			};

			std::string update_status_text();

			typedef uint32_t notify_flags;

			qualified_path parent_id() const;
			void notify_parent(notify_flags flags) const;

		public:
			void notify(qualified_path const& source, notify_flags flags);

		protected:
			param_tree* m_tree;
			Wt::WContainerWidget* m_container;
			par_wgt_holder* m_holder;
			qualified_path m_path;
			ParamType m_param_type;
			param_wgt_impl* m_impl;
			qualified_path m_id;	// TODO: Or store complete schema component?
			std::string m_filter;
			boost::optional< std::string > m_import_source;
			bool m_import_modified;
			Wt::WPopupMenu* m_menu;
			std::map< MenuItemId, Wt::WMenuItem* > m_menu_items;

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

	typedef param_tree::param_wgt_impl param_wgt_impl;
	
}


#endif


