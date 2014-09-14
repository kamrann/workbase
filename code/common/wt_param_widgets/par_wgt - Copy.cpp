// par_wgt.cpp

#include "par_wgt.h"

#include "boolean_par_wgt.h"
#include "integer_par_wgt.h"
#include "realnum_par_wgt.h"
#include "string_par_wgt.h"
#include "enum_par_wgt.h"
#include "vector_par_wgt.h"
#include "random_par_wgt.h"

#include "container_par_wgt.h"
#include "repeat_par_wgt.h"

#include "par_wgt_holder.h"

#include "pw_yaml.h"
#include "param_accessor.h"

#include <Wt/WText>
#include <Wt/WContainerWidget>
#include <Wt/WFormWidget>
#include <Wt/WGroupBox>
#include <Wt/WPushButton>
#include <Wt/WPopupMenu>
#include <Wt/WVBoxLayout>
#include <Wt/WHBoxLayout>
#include <Wt/WDialog>
#include <Wt/WLineEdit>
#include <Wt/WBreak>

#include <boost/variant/static_visitor.hpp>

#include <fstream>
#include <regex>
#include <filesystem>


namespace prm
{
#if 0
	std::string param_tree::get_leaf_id(std::string const& qual_name)
	{
		auto pos = qual_name.find_last_of(IdScopeString);
		if(pos == std::string::npos)
		{
			return qual_name;
		}
		else
		{
			return qual_name.substr(pos + IdScopeString.length());
		}
	}

	std::string param_tree::strip_instances(std::string const& name)
	{
		auto result = name;
		auto pos = result.find('[');
		while(pos != std::string::npos)
		{
			auto close = result.find(']', pos + 1);
			assert(close != std::string::npos);

			result.erase(pos, close - pos + 1);
			pos = result.find('[', pos);
		}
		return result;
	}

	// Checks that two ids have the same repeat indexes, up to the point that their base id components diverge
	// Assumes both arguments are fully qualified
	bool param_tree::match_indexes(std::string const& str1, std::string const& str2)
	{
		std::string::size_type pos = 0;
		/*
				basically, each section is of the form : >name[i] with the brackets optional.
				if name1 != name2 then return success
				else if no brackets or i1 == i2 then continue to next section
				else return failure.
		*/

		while(pos != std::string::npos)
		{
			assert(str1.find(IdScopeString, pos) == pos);
			assert(str2.find(IdScopeString, pos) == pos);

			// Find end of this component (start at pos+IdScopeString.length() since assuming we have IdScopeString at inde pos)
			auto end1 = str1.find(IdScopeString, pos + IdScopeString.length());
			auto end2 = str2.find(IdScopeString, pos + IdScopeString.length());
			// Isolate this component
			auto sub1 = str1.substr(pos, end1 == std::string::npos ? end1 : (end1 - pos));
			auto sub2 = str2.substr(pos, end2 == std::string::npos ? end2 : (end2 - pos));
			// Set next pos value (in cases where it will be used, we know end1 = end2)
			pos = end1;
			// Ignore starting scope string
			sub1 = sub1.substr(IdScopeString.length());
			sub2 = sub2.substr(IdScopeString.length());
			// Look for opening brackets
			auto brk1 = sub1.find('[');
			auto brk2 = sub2.find('[');
			// If found, isolate proceeding name section
			auto name1 = brk1 == std::string::npos ? sub1 : sub1.substr(0, brk1);
			auto name2 = brk2 == std::string::npos ? sub2 : sub2.substr(0, brk2);

			// Are both names identical?
			if(name1.compare(name2) != 0)
			{
				// No, diverged, so no need for further comparison
				return true;
			}
			else
			{
				// Yes
				assert(brk1 == brk2);

				// Did we find brackets?
				if(brk1 == std::string::npos)
				{
					// No, so no index on this component, proceed
					continue;
				}
				else
				{
					// Yes, so find closing brackets
					auto brk_close1 = sub1.find(']', brk1 + 1);
					auto brk_close2 = sub2.find(']', brk2 + 1);
					assert(brk_close1 != std::string::npos);
					assert(brk_close2 != std::string::npos);

					// Extract indexes
					auto idx1 = sub1.substr(brk1 + 1, brk_close1 - brk1 - 1);
					auto idx2 = sub2.substr(brk2 + 1, brk_close2 - brk2 - 1);
					assert(!idx1.empty());
					assert(!idx2.empty());

					// Are they the same?
					if(idx1.compare(idx2) == 0)
					{
						// Yes, proceed
						continue;
					}
					else
					{
						// No, indexes differ, match failed
						return false;
					}
				}
			}
		}

		// Reached end of strings without diverging, so match successful
		return true;
	}
#endif

	param_tree::param_tree()
	{
#ifdef _DEBUG
		std::cerr << "PARAM_TREE CONSTRUCT" << std::endl;
#endif
	}

	param_tree::~param_tree()
	{
#ifdef _DEBUG
		std::cerr << "param_tree destruct" << std::endl;
#endif

		// Force existing implementation (root param_wgt and all its descendents) to be destroyed now,
		// so they get unregistered before the widget and signal maps are destroyed.
		setImplementation(new Wt::WContainerWidget());
	}

	param_tree* param_tree::create(schema::schema_provider schema_provider)
	{
		auto tree = new param_tree();

		// Register default schema accessor for first time initialisation
		tree->m_schema_accessor = [schema_provider](qualified_path const& subschema_name, param_accessor acc)
		{
			return schema_provider(subschema_name, acc);
		};

		// Create widget tree from default schema
		auto root_path = qualified_path();
		auto default_schema = schema_provider(root_path, param_accessor());
		auto root = tree->create_widget(default_schema, root_path);
		// Register root widget
		tree->setImplementation(root);
//		tree->
		// And finally, register proper schema accessor
		tree->m_schema_accessor = [schema_provider, tree](qualified_path const& subschema_name, param_accessor acc)
		{
			// TODO: By using subschema as ignore path, we are preventing the schema provider from 
			// finding param values in or descending from the path to the schema we are requesting.
			// This precludes schema objects from recreating themselves.
			// If don't want this restriction, need some other way to prevent name resolution from 
			// accidentally finding a value it shouldn't, as was happening in the case of the value_type
			// for a single manual objective being found when changing to pareto multi objective.
//			auto prm_acc = param_accessor(tree->get_yaml_param(), subschema_name, subschema_name);

			return schema_provider(subschema_name, acc);// prm_acc);
		};

		return tree;
	}

	param_tree::param_wgt* param_tree::create_widget(
		schema::schema_node const& schema,
		qualified_path const& qual_path)
	{
		auto wgt = param_wgt::create(schema, qual_path, this);

#ifdef _DEBUG
		std::cerr << "REGISTER (" << wgt->get_id() << ")" << std::endl;
#endif

		assert(m_widgets.find(wgt->get_id()) == m_widgets.end());
		m_widgets[wgt->get_id()] = wgt;
#if 0
		if(schema["trigger"])
		{
			auto events = schema["trigger"].as< std::vector< std::string > >();
			for(auto ev : events)
			{
				// TODO: not just changed handler...
				wgt->connect_changed_handler([this, ev, wgt]
				{
					trigger_event(ev, wgt->get_id());
				});
			}
		}

		if(schema["update on"])
		{
			auto events = schema["update on"].as< std::vector< std::string > >();
			for(auto ev : events)
			{
				bool absolute = false;
				if(ev[0] == '$')
				{
					absolute = true;
					ev = ev.substr(1);
				}
				m_listeners[ev].insert({ wgt->get_id(), absolute });
			}
		}
#endif

		return wgt;
	}

	void param_tree::unregister_widget(param_wgt* wgt)
	{
#ifdef _DEBUG
		std::cerr << "unregister (" << wgt->get_id() << ")" << std::endl;
#endif

		auto id = wgt->get_id();
		auto w_it = m_widgets.find(id);
		assert(w_it != m_widgets.end());
		m_widgets.erase(w_it);

		for(auto& ev_listeners : m_listeners)
		{
			auto it = ev_listeners.second.find(id);
			if(it != ev_listeners.second.end())
			{
				ev_listeners.second.erase(it);
			}
		}

		auto c_it = m_connections.find(id);
		if(c_it != m_connections.end())
		{
			auto conns = c_it->second;
			for(auto const& conn : conns)
			{
				conn.disconnect();
			}
			m_connections.erase(c_it);
		}
	}

	bool param_tree::register_changed_handler(qualified_path const& trigger_id, qualified_path const& recipient_id, std::function< void() > handler)
	{
		auto trigger_it = m_widgets.find(trigger_id);
		assert(trigger_it != m_widgets.end());
		auto trigger = trigger_it->second;
		m_connections[recipient_id].insert(trigger->connect_changed_handler(handler));
		return true;
	}

	void param_tree::update_widget_from_schema(qualified_path const& wgt_id)
	{
		auto w_it = m_widgets.find(wgt_id);
		assert(w_it != m_widgets.end());
		auto wgt = w_it->second;
		auto schema = get_schema(wgt_id);//m_schema_accessor(wgt_id);

		assert(schema["type"].as< ParamType >() != ParamType::Repeat);	// TODO: This type currently unsupported as update target

		wgt->initialize(schema, wgt_id.stem(), this);
	}

	void param_tree::trigger_event(std::string const& event_name, qualified_path const& trigger_path)
	{
		auto listeners_it = m_listeners.find(event_name);
		if(listeners_it == m_listeners.end())
		{
			return;
		}

		auto& listeners = listeners_it->second;
		for(auto const& elem : listeners)
		{
			auto const& id = elem.first;
			auto const& options = elem.second;
			// TODO: 
			if(options ||
				qualified_path::match_indexes(id, trigger_path)
				)
			{
				update_widget_from_schema(id);
			}
		}
	}

	param param_tree::get_yaml_param(bool instantiated)
	{
		auto root = static_cast<param_wgt const*>(implementation());
		return root == nullptr ? param() : root->get_yaml_param(instantiated);
	}

	void param_tree::set_from_yaml_param(param const& vals)
	{
		auto root = static_cast<param_wgt*>(implementation());
		root->set_from_yaml_param(vals);
	}

	schema::schema_node param_tree::get_schema(qualified_path const& schema_name)//, boost::optional< param > param_vals)
	{
		auto params = m_extended_params ? *m_extended_params : get_yaml_param(true);
		auto prm_acc = param_accessor(
			params,
			schema_name,
			/* TODO: hack, see note in param_tree::create() */
			m_extended_params ? boost::none : boost::optional< qualified_path >(schema_name));
		return m_schema_accessor(schema_name, prm_acc);
	}

	param_accessor param_tree::get_param_accessor(qualified_path const& wrt)
	{
		return param_accessor(get_yaml_param(true), /*this,*/ wrt);
	}

	qualified_path param_tree::resolve_id(
		std::string const& id,
		boost::optional< qualified_path > const& parallel) const
	{
		// TODO: For now assuming id cannot be fully qualified (ie. doesn't start with a '>') !!!!!!
/*		if(m_widgets.find(id) != m_widgets.end())
		{
			return id;
		}
*/
		// TODO: Walk order??
		for(auto const& elem : m_widgets)
		{
			auto const& w_id = elem.first;

			auto it = w_id.find(id);
			if(it != w_id.end())
			{
				// TODO: Currently assuming id is entirely unqualified - leaf only
				if(!parallel ||									// Not asked for parallel match, or
					qualified_path::match_indexes(w_id, *parallel))		// Successful parallel match
				{
					// Found
					return w_id;
				}
				// else, failed to match, so continue
			}

/*			auto off = w_id.find(id);
			if(off != std::string::npos && off + id.length() == w_id.length())
			{
				// TODO: Currently only doing parallel check if searched-for id is entirely unqualified
				// (ie. contains no '>'s). Not yet sure if this is what is wanted though...
				if(id.find(IdScopeString) != std::string::npos ||	// Is qualified, or
					!parallel ||									// Not asked for parallel match, or
					match_indexes(w_id, *parallel))					// Successful parallel match
				{
					// Found
					return w_id;
				}
				// else, failed to match, so continue
			}
*/		}

		assert(false);
		return qualified_path();	// todo: failure
	}

	param_tree::param_wgt* param_tree::find_widget(qualified_path const& path) const
	{
		auto it = m_widgets.find(path);
		if(it == m_widgets.end())
		{
			return nullptr;
		}
		else
		{
			return it->second;
		}
	}


	param_tree::param_wgt::param_wgt(): m_base_impl(nullptr)
	{

	}

	param_tree::param_wgt::~param_wgt()
	{
		m_tree->unregister_widget(this);
	}

	/********/

	std::string import_title_from_filename(std::string filename, std::string filter_str)
	{
		// Escape the filter string for regex use
		std::regex rgx("\\.");
		filter_str = std::regex_replace(filter_str, rgx, "\\.");
		// Then append .<match>.yaml
		filter_str += "\\.([-_ [:alnum:]]+)\\.yaml";
		rgx = std::regex(filter_str);
		// And match
		std::smatch mr;
		assert(std::regex_match(filename, mr, rgx));
		return mr[1].str();
	}

	std::string import_filename_from_title(std::string title, std::string filter_str)
	{
		// Prepend <filter>. and append .yaml
		return filter_str + "." + title + ".yaml";
	}

	namespace fs = std::tr2::sys;

	std::vector< std::pair< std::string, std::string > > get_importable_schemas(std::string filter_str)
	{
		std::vector< std::pair< std::string, std::string > > schemas;
		std::regex rgx("\\.");
		filter_str = std::regex_replace(filter_str, rgx, "\\.");
		filter_str += "\\.([-_ [:alnum:]]+)\\.yaml";
		rgx = std::regex(filter_str);

		auto path = fs::current_path< fs::path >();
		for(auto it = fs::directory_iterator(path); it != fs::directory_iterator(); ++it)
		{
			auto const& file = it->path();

			std::smatch mr;
			auto filename = file.filename();
			if(!fs::is_directory(file) && std::regex_match(filename, mr, rgx))
			{
				schemas.emplace_back(std::make_pair(mr[1].str(), filename));
			}
		}

		return schemas;
	}

	void replace_param_subtree(param param_vals, qualified_path path, param sub_param)
	{
		auto node = find_value(param_vals, path);
		node = sub_param[ParamNode::Value];
	}
	/********/

	void param_tree::param_wgt::initialize(schema::schema_node const& script, qualified_path const& qual_path, param_tree* tree)
	{
		bool first_time = (m_base_impl == nullptr);
		auto qual_id = qual_path + script["name"].as< std::string >();
		if(first_time)
		{
			m_id = qual_id;
			m_container = new Wt::WContainerWidget;
			setImplementation(m_container);
		}
		else
		{
			assert(m_id == qual_id);
			// todo: assert(?my type? == script["type"]);

			m_base_impl = nullptr;
			m_container->clear();

			m_menu = nullptr;
			m_menu_items.clear();
			//m_status_text = nullptr;
		}

		auto base_layout = new Wt::WVBoxLayout;
		base_layout->setContentsMargins(0, 0, 0, 0);
		m_container->setLayout(base_layout);

		m_base_impl = create_impl(script);

		auto has_border = false;
		boost::optional< std::string > label = boost::none;
		bool import_enabled = script["import"];
		
		auto visual = script["visual"];
		if(visual)
		{
			has_border = visual["border"] && visual["border"].as< bool >();
			if(auto& label_node = visual["label"])
			{
				label = label_node.as< std::string >();
			}
		}

		if(!is_leaf_type(script["type"].as< ParamType >())
			&& (label || import_enabled))
		{
			m_holder = new boxed_holder();
		}
		else
		{
			m_holder = new inline_holder();
		}

		if(label)
		{
			m_holder->set_label(*label);
		}
		m_holder->enable_border(has_border);
		m_holder->set_contents(m_base_impl);

		base_layout->addWidget(m_holder, 0, Wt::AlignLeft | Wt::AlignTop);

		////////////
		if(import_enabled)
		{
			auto on_import_item = [=](std::pair< std::string, std::string > imported, Wt::WMenuItem* item, Wt::NoClass, Wt::NoClass, Wt::NoClass, Wt::NoClass, Wt::NoClass)
			{
				auto title = imported.first;
				auto filename = imported.second;

				auto import_param = YAML::LoadFile(filename);

				// Use fully include-instantiated param to generate schema
				auto instantiated = instantiate_includes(import_param, true);
				param param_vals = m_tree->get_yaml_param(true);
				replace_param_subtree(param_vals, m_id, instantiated);

#ifdef _DEBUG
				std::cout << "After merge:" << std::endl << param_vals << std::endl;
#endif

				m_import = title;
				m_tree->m_extended_params = param_vals;
				auto subschema = m_tree->get_schema(m_id);

				initialize(subschema, m_id.stem(), m_tree);

				// But pass uninstantiated version to set_from_yaml_param, so that each level of
				// param widget can detect if it should be set to import status.
				set_from_yaml_param(import_param);
				m_tree->m_extended_params = boost::none;
			};

			m_filter = script["import"].as< std::string >();
			if(m_filter.empty())
			{
				m_filter = m_id.unindexed().to_string(".", false);
			}
			auto importable = get_importable_schemas(m_filter);

			m_menu = new Wt::WPopupMenu;

			auto modify_item = m_menu->addItem("Modify");
			m_menu_items[MenuItemId::Modify] = modify_item;
			modify_item->triggered().connect([=](Wt::WMenuItem* item, Wt::NoClass, Wt::NoClass, Wt::NoClass, Wt::NoClass, Wt::NoClass)
			{
				m_import = boost::none;
				m_menu->setItemHidden(m_menu_items[MenuItemId::Modify], true);
				m_menu->setItemHidden(m_menu_items[MenuItemId::SaveAs], false);
				auto status = update_status_text();
				m_holder->set_status_text(status);
			});
			m_menu->setItemHidden(modify_item, !m_import);

			auto import_submenu = new Wt::WPopupMenu;
			for(auto const& entry : importable)
			{
				auto title = entry.first;
				auto filename = entry.second;

				auto item = import_submenu->addItem(title);
				item->triggered().connect([on_import_item, entry](Wt::WMenuItem* item, Wt::NoClass, Wt::NoClass, Wt::NoClass, Wt::NoClass, Wt::NoClass)
				{
					on_import_item(entry, item, Wt::NoClass(), Wt::NoClass(), Wt::NoClass(), Wt::NoClass(), Wt::NoClass());
				});
			}
			m_menu->addMenu("Import", import_submenu);

			auto save_item = m_menu->addItem("Save As");
			m_menu_items[MenuItemId::SaveAs] = save_item;
			save_item->triggered().connect([=](Wt::WMenuItem* item, Wt::NoClass, Wt::NoClass, Wt::NoClass, Wt::NoClass, Wt::NoClass)
			{
				auto dlg = new Wt::WDialog("Save param block");
				auto v_layout = new Wt::WVBoxLayout;
				dlg->contents()->setLayout(v_layout);

				auto edit = new Wt::WLineEdit();
				edit->setWidth(200);
				// TODO: validator accepting only alpha numeric and underscore
				v_layout->addWidget(edit);
				v_layout->addWidget(new Wt::WBreak());
				auto h_layout = new Wt::WHBoxLayout;
				v_layout->addLayout(h_layout);
				auto ok_btn = new Wt::WPushButton("Save");
				h_layout->addWidget(ok_btn, 1);
				auto cancel_btn = new Wt::WPushButton("Cancel");
				h_layout->addWidget(cancel_btn, 0);

				edit->enterPressed().connect(dlg, &Wt::WDialog::accept);
				ok_btn->clicked().connect(dlg, &Wt::WDialog::accept);
				cancel_btn->clicked().connect(dlg, &Wt::WDialog::reject);

				if(dlg->exec() == Wt::WDialog::Accepted)
				{
					auto save_title = edit->text().toUTF8();
					auto save_filename = import_filename_from_title(save_title, m_filter);
					std::ofstream file(save_filename, std::ios_base::out | std::ios_base::trunc);
					file << get_yaml_param(false);
					file.close();

					m_import = save_title;
					m_menu->setItemHidden(m_menu_items[MenuItemId::SaveAs], true);
					m_menu->setItemHidden(m_menu_items[MenuItemId::Modify], false);
					auto status = update_status_text();
					m_holder->set_status_text(status);
				}

				delete dlg;
			});
			m_menu->setItemHidden(save_item, m_import);

//			m_status_text = new Wt::WText();
			auto status = update_status_text();
//			base_layout->addWidget(m_status_text);
			m_holder->set_status_text(status);

//			auto menu_btn = new Wt::WPushButton();
//			menu_btn->setStyleClass("dropbutton");
//			menu_btn->setMenu(m_menu);
//			base_layout->addWidget(menu_btn);// , 0, Wt::AlignRight | Wt::AlignTop);
			m_holder->set_menu(m_menu);
		}
		////////////

		if(script["trigger"])
		{
			auto events = script["trigger"].as< std::vector< std::string > >();
			for(auto ev : events)
			{
				// TODO: not just changed handler...
				connect_changed_handler([this, ev, tree]
				{
					tree->trigger_event(ev, get_id());
				});
			}
		}

		if(script["update on"])
		{
			auto events = script["update on"].as< std::vector< std::string > >();
			for(auto ev : events)
			{
				bool absolute = false;
				if(ev[0] == '$')
				{
					absolute = true;
					ev = ev.substr(1);
				}
				tree->m_listeners[ev].insert({ get_id(), absolute });
			}
		}
	}

	param_tree::param_wgt* param_tree::param_wgt::create(
		schema::schema_node const& schema,
		qualified_path const& qual_path,
		param_tree* tree)
	{
		assert(schema.IsMap());

		auto type = schema["type"].as< prm::ParamType >();

		param_wgt* pw = nullptr;
		switch(type)
		{
			case ParamType::Boolean:
			pw = new boolean_par_wgt();
			break;

			case ParamType::Integer:
			pw = new integer_par_wgt();
			break;

			case ParamType::RealNumber:
			pw = new realnum_par_wgt();
			break;

			case ParamType::String:
			pw = new string_par_wgt();
			break;

			case ParamType::Enumeration:
			pw = new enum_par_wgt();
			break;

			case ParamType::Vector2:
			pw = new vector_par_wgt();
			break;

			case ParamType::Random:
			pw = new random_par_wgt();
			break;

			case ParamType::List:
			pw = new container_par_wgt();
			break;

			case ParamType::Repeat:
			pw = new repeat_par_wgt();
			break;
		}

		if(pw == nullptr)
		{
			return nullptr;
		}

		pw->m_tree = tree;
		pw->initialize(schema, qual_path, tree);
		return pw;
	}

	std::string param_tree::param_wgt::update_status_text()
	{
		std::string import_label_text;
		import_label_text += "[";
		if(m_import)
		{
			import_label_text += *m_import;
		}
		else
		{
			import_label_text += "custom";
		}
		import_label_text += "]";

		//m_status_text->setText(import_label_text);
		return import_label_text;
	}

	void param_tree::param_wgt::set_from_yaml_param(param const& p)
	{
		if(m_menu != nullptr)	// TODO: Hack to test if importing is enabled on this widget
		{
			if(p.Tag().compare("!import") == 0)
			{
				auto source = p.as< std::string >();

				m_import = import_title_from_filename(source, m_filter);
				m_menu->setItemHidden(m_menu_items[MenuItemId::SaveAs], true);
				m_menu->setItemHidden(m_menu_items[MenuItemId::Modify], false);
				auto status = update_status_text();
				m_holder->set_status_text(status);
			}
		}

		auto instantiated = instantiate_includes(p, false);
		update_impl_from_yaml_param(instantiated);
	}

	void param_tree::param_wgt::on_save_as()
	{

	}

	void param_tree::param_wgt::on_modify()
	{

	}

	Wt::Signals::connection param_tree::param_wgt::connect_changed_handler(std::function< void() > const& handler)
	{
		Wt::WFormWidget* fw = dynamic_cast<Wt::WFormWidget*>(m_base_impl);
		assert(fw != nullptr);
		return fw->changed().connect(std::bind(handler));
	}

	param param_tree::param_wgt::get_yaml_param(bool instantiated) const
	{
		if(m_import)
		{
			if(instantiated)
			{
				return YAML::LoadFile(import_filename_from_title(*m_import, m_filter));
			}
			else
			{
				param node;
				node = import_filename_from_title(*m_import, m_filter);
				node.SetTag("!import");
				return node;
			}
		}
		else
		{
			return get_locally_instantiated_yaml_param(instantiated);
		}
	}

	std::string param_tree::param_wgt::get_yaml_script(bool instantiated) const
	{
		return YAML::Dump(get_yaml_param(instantiated));
	}
}




