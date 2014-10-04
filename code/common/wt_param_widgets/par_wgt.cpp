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

#include "params/param_yaml.h"
#include "params/param_accessor.h"

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
//		std::cerr << "REGISTER (" << wgt->get_id() << ")" << std::endl;
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
//		std::cerr << "unregister (" << wgt->get_id() << ")" << std::endl;
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

	bool param_tree::register_handler(
		qualified_path const& trigger_id,
		qualified_path const& recipient_id,
		pw_event::type type,
		pw_event_handler const& handler)
	{
		auto trigger_it = m_widgets.find(trigger_id);
		assert(trigger_it != m_widgets.end());
		auto trigger = trigger_it->second;
		m_connections[recipient_id].insert(trigger->connect_handler(type, handler));
		return true;
	}

	void param_tree::update_widget_from_schema(qualified_path const& wgt_id)
	{
		auto w_it = m_widgets.find(wgt_id);
		assert(w_it != m_widgets.end());
		auto wgt = w_it->second;

		// First, get current param val
		auto current = wgt->get_yaml_param(false);

		// Now reinitialize from newly acquired schema
		auto schema = get_schema(wgt_id);
		wgt->initialize(schema, false);

		// Try to retain values
		bool retained = wgt->set_from_yaml_param(current);

		if(!retained)
		{
			if(wgt->m_import_source && !wgt->m_import_modified)
			{
				wgt->m_import_modified = true;
				auto status = wgt->update_status_text();
				wgt->m_holder->set_status_text(status);
				wgt->m_menu->setItemHidden(wgt->m_menu_items[param_wgt::MenuItemId::SaveAs], false);
			}

			wgt->notify_parent(0);
			// TODO: way to invoke emit("changed") // m_changed_event.emit(changed());
		}
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
				//qualified_path::match_indexes(id, trigger_path)
				id.is_unambiguous_from(trigger_path)
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

	bool param_tree::set_from_yaml_param(param const& vals)
	{
		auto root = static_cast<param_wgt*>(implementation());
		return root->set_from_yaml_param(vals);
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
					//qualified_path::match_indexes(w_id, *parallel))		// Successful parallel match
					w_id.is_unambiguous_from(*parallel))
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
						*/
		}

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


	param_tree::param_wgt::param_wgt(qualified_path const& path, param_tree* tree):
		m_path(path),
		m_tree(tree),
		m_holder(nullptr),
		m_param_type(ParamType::None),
		m_impl(nullptr),
		m_import_source(),
		m_import_modified(false),
		m_menu(nullptr)
	{
		m_container = new Wt::WContainerWidget;
		setImplementation(m_container);
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
		auto node = find_node(param_vals, path);
		node[ParamNode::Value] = sub_param[ParamNode::Value];
	}
	/********/

	void param_tree::param_wgt::reset(bool clear_import)
	{
		if(m_param_type != ParamType::None)
		{
			m_param_type = ParamType::None;
			m_id = qualified_path();
			m_filter = std::string{};

			m_impl = nullptr;
			m_container->clear();

			if(clear_import)
			{
				//m_import_source = boost::none;
				m_import_modified = true;
			}

			// TODO: Could detach menu in case when clear_import is false, so that it is not destroyed by
			// m_container->clear() call.
			m_menu = nullptr;
			m_menu_items.clear();
		}
	}

	void param_tree::param_wgt::initialize(schema::schema_node /*const&*/ script, bool clear_import)
	{
		reset(clear_import);

		m_id = m_path + script["name"].as< std::string >();

/*		bool already_readonly = script["readonly"] && script["readonly"].as< bool >();
		script["readonly"] = already_readonly || m_import;
*/
		auto type = script["type"].as< prm::ParamType >();
		switch(type)
		{
			case ParamType::Boolean:
			m_impl = boolean_par_wgt::create(script);
			break;

			case ParamType::Integer:
			m_impl = integer_par_wgt::create(script);
			break;

			case ParamType::RealNumber:
			m_impl = realnum_par_wgt::create(script);
			break;

			case ParamType::String:
			m_impl = string_par_wgt::create(script);
			break;

			case ParamType::Enumeration:
			m_impl = enum_par_wgt::create(script);
			break;

			case ParamType::Vector2:
			m_impl = vector_par_wgt::create(script);
			break;

			case ParamType::Random:
			m_impl = random_par_wgt::create(script);
			break;

			case ParamType::List:
			m_impl = container_par_wgt::create(script, m_id, m_tree);
			break;

			case ParamType::Repeat:
			m_impl = repeat_par_wgt::create(script, m_id, m_tree);
			break;

			default:
			throw std::exception("Invalid Param Type for param_wgt");
		}

		m_param_type = type;

		auto base_layout = new Wt::WVBoxLayout;
		base_layout->setContentsMargins(0, 0, 0, 0);
		m_container->setLayout(base_layout);

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
		m_holder->set_contents(m_impl->get_wt_widget());

		base_layout->addWidget(m_holder);// , 0, Wt::AlignLeft | Wt::AlignTop);

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
//				std::cout << "After merge:" << std::endl << param_vals << std::endl;
#endif

				m_import_source = title;
				m_import_modified = false;
				m_tree->m_extended_params = param_vals;
				auto subschema = m_tree->get_schema(m_id);

				initialize(subschema, false);

				// But pass uninstantiated version to set_from_yaml_param, so that each level of
				// param widget can detect if it should be set to import status.
				// TODO: not ideal, we are basically setting up the import status and menu twice, once in 
				// above initialize() call, and again in following call...
				set_from_yaml_param(import_param, m_import_source);
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
/*				m_import_mo = boost::none;
				m_menu->setItemHidden(m_menu_items[MenuItemId::Modify], true);
				m_menu->setItemHidden(m_menu_items[MenuItemId::SaveAs], false);
				auto status = update_status_text();
				m_holder->set_status_text(status);
*/			});
			m_menu->setItemHidden(modify_item, true);// !m_import);

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
				if(m_import_source)
				{
					edit->setText(*m_import_source);
				}
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

					m_import_source = save_title;
					m_import_modified = false;
					m_menu->setItemHidden(m_menu_items[MenuItemId::SaveAs], true);
					m_menu->setItemHidden(m_menu_items[MenuItemId::Modify], true);// false);
					auto status = update_status_text();
					m_holder->set_status_text(status);
				}

				delete dlg;
			});
			m_menu->setItemHidden(save_item, m_import_source && !m_import_modified);

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
				// TODO: event types
				connect_handler("changed", [this, ev]
				{
					m_tree->trigger_event(ev, get_id());
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
				m_tree->m_listeners[ev].insert({ get_id(), absolute });
			}
		}
	}

	param_tree::param_wgt* param_tree::param_wgt::create(
		schema::schema_node const& schema,
		qualified_path const& qual_path,
		param_tree* tree)
	{
		auto pw = new param_wgt(qual_path, tree);
		if(pw == nullptr)
		{
			return nullptr;
		}

		pw->initialize(schema, false);// true);
		return pw;
	}

	std::string param_tree::param_wgt::update_status_text()
	{
		std::string import_label_text;
		import_label_text += "[";
		if(m_import_source)
		{
			import_label_text += *m_import_source;
			if(m_import_modified)
			{
				import_label_text += "*";
			}
		}
		else
		{
			import_label_text += "custom";
		}
		import_label_text += "]";

		//m_status_text->setText(import_label_text);
		return import_label_text;
	}

	bool param_tree::param_wgt::set_from_yaml_param(param const& p, boost::optional< std::string > import)
	{
//		bool import_state_changed = false;
		if(m_menu != nullptr)	// TODO: Temp hack to detect if import is enabled
			// note that currently making the assumption that import tags on loaded yaml files
			// will never be set for a param widget that doesn't have import enabled schema!!!
		{
			if(import)
			{
				m_import_source = import;
			}
			else if(p.Tag().compare("!import") == 0)
			{
/*				if(!m_import)
				{
					import_state_changed = true;
				}
*/
				auto source = p.as< std::string >();
				m_import_source = import_title_from_filename(source, m_filter);
				m_import_modified = false;
			}
			else
			{
				m_import_source = boost::none;
				m_import_modified = false;
			}
			m_menu->setItemHidden(m_menu_items[MenuItemId::SaveAs], m_import_source && !m_import_modified);
			m_menu->setItemHidden(m_menu_items[MenuItemId::Modify], true);// !m_import);
			auto status = update_status_text();
			m_holder->set_status_text(status);
		}

/*		if(import_state_changed)
		{
			// This full reinitialize probably overkill, but need to recreate implementation 
			// for new readonly status.
			auto subschema = m_tree->get_schema(m_id);
			initialize(subschema, false);
		}
*/
		auto instantiated = instantiate_includes(p, false);

		if(instantiated[ParamNode::Name].as< std::string >() != get_id().leaf().name() ||
			instantiated[ParamNode::Type].as< ParamType >() != m_param_type)
		{
			if(m_import_source)
			{
				throw std::exception("Import mismatch");
			}
			else
			{
				return false;
			}
		}

		auto content = instantiated[ParamNode::Value];
		return m_impl->update_impl_from_yaml_param(content);
	}

	qualified_path param_tree::param_wgt::parent_id() const
	{
		return get_id().stem();
	}

	void param_tree::param_wgt::notify_parent(notify_flags flags) const
	{
		if(!get_id().is_leaf())
		{
			auto p_id = parent_id();
			m_tree->m_widgets[p_id]->notify(get_id(), flags);
		}
	}

	void param_tree::param_wgt::notify(qualified_path const& source, notify_flags flags)
	{
		// TODO: For now assuming source is a child
		if(m_import_source && !m_import_modified)
		{
			m_import_modified = true;
			auto status = update_status_text();
			m_holder->set_status_text(status);
			m_menu->setItemHidden(m_menu_items[MenuItemId::SaveAs], false);
		}
	}

	Wt::Signals::connection param_tree::param_wgt::connect_handler(pw_event::type type, pw_event_handler const& handler)
	{
		return m_impl->connect_handler(type, handler);
	}

	param param_tree::param_wgt::get_yaml_param(bool instantiated) const
	{
		if(m_import_source && !m_import_modified)
		{
			if(instantiated)
			{
				auto node = YAML::LoadFile(import_filename_from_title(*m_import_source, m_filter));
				return instantiate_includes(node, true);
			}
			else
			{
				param node;
				node = import_filename_from_title(*m_import_source, m_filter);
				node.SetTag("!import");
				return node;
			}
		}
		else
		{
			param node;
			node[ParamNode::Name] = m_id.leaf().name();
			node[ParamNode::Type] = m_param_type;
			node[ParamNode::Value] = m_impl->get_locally_instantiated_yaml_param(instantiated);
			return node;
		}
	}

	std::string param_tree::param_wgt::get_yaml_script(bool instantiated) const
	{
		return YAML::Dump(get_yaml_param(instantiated));
	}
}




