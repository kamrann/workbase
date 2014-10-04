// cmdline.cpp

#include "cmdline.h"
#include "cmd_parser.h"
#include "schema.h"

#include "value_emitter.h"

#include <boost/variant/static_visitor.hpp>


namespace prm {

	cmdline_processor::cmdline_processor(
		schema::schema_provider_map_handle provider,
		param_tree& pt):
		m_provider{ std::move(provider) },
		m_pt{ pt },
		m_accessor{ &m_pt }
	{}

	class cmd_visitor: public boost::static_visitor < bool >
	{
	public:
		cmd_visitor(cmdline_processor& _prc, std::ostream& _os):
			prc(_prc),
			os(_os)
		{}

		inline qualified_path nav_cmd_path(nav_cmd const& cmd) const
		{
			qualified_path path{};
			switch(cmd.dir)
			{
				case nav_cmd::Direction::Up:
				if(cmd.elem)
				{
					path = prc.m_accessor.find_path_ancestor(*cmd.elem);
					if(!path)
					{
						os << "no ancestor \'" << *cmd.elem << "\'" << std::endl;
					}
				}
				else
				{
					path = prc.m_accessor.where();
					if(path.size() == 1)
					{
						os << "no parent" << std::endl;
					}
					else
					{
						path.pop();
					}
				}
				break;

				case nav_cmd::Direction::Down:
				{
					path = prc.m_accessor.find_path_descendent(*cmd.elem);
					if(!path)
					{
						os << "no descendent \'" << *cmd.elem << "\', or ambiguous" << std::endl;
					}
				}
				break;
			}

			return path;
		}

		inline std::ostream& output_value(param val)
		{
//			typedef std::back_insert_iterator< std::string > sink_type;
//			sink_type sink(dest);
//			karma::generate(sink, grammar, value(s));

			value_emitter< karma::ostream_iterator< char > > em;
			os << karma::format(em, val);

			if(!os)
			{
				os.clear();
				os << std::endl << "!! output failure !!" << std::endl;
			}

			return os;
		}

		inline std::ostream& output_constraints(schema::schema_node sch)
		{
			auto type = sch["type"].as< ParamType >();
			switch(type)
			{
				case ParamType::Enumeration:
				{
					os << "in { ";
					for(auto node : sch["constraints"]["values"])
					{
						// TODO: shouldn't this be name rather than id? likewise for parsing/emitting
						os << node["id"] << " ";
					}
					os << "}";
				}
				break;
			}

			return os;
		}

		inline std::ostream& output_type_info(param_tree::param_data const& pd)
		{
			// todo: constraints
			os << "[" << ParamTypeNameMap.left.at(pd.type) << "]";
			return os;
		}

		inline result_type operator() (nav_cmd const& cmd)
		{
			qualified_path new_path = nav_cmd_path(cmd);
			if(new_path)
			{
				prc.m_accessor.move_to(new_path);
				return true;
			}
			else
			{
				return false;
			}
		}

		inline result_type operator() (list_cmd const& cmd)
		{
			qualified_path path;
			param_tree::param_data pd;
			if(cmd.nav)
			{
				path = nav_cmd_path(*cmd.nav);
				if(!path)
				{
					return false;
				}
				pd = prc.m_accessor.find_param(path);
			}
			else
			{
				path = prc.m_accessor.where();
				pd = prc.m_accessor.param_here();
			}

			switch(pd.type)
			{
				case ParamType::List:
				{
					os << "..." << std::endl;
					auto child_list = prc.m_accessor.children();
					for(auto c_path : child_list)
					{
						auto c_pd = prc.m_accessor.find_param(c_path);
						os << c_pd.name << ": ";
						switch(c_pd.type)
						{
							case ParamType::List:
							case ParamType::Repeat:
							os << "...";
							break;

							default:
							output_value(*c_pd.value);
							if(cmd.type_info)
							{
								os << " ";
								output_type_info(c_pd);
								os << " ";
								output_constraints((*prc.m_provider)[c_path](prc.m_accessor));
							}
							break;
						}
						os << std::endl;
					}
				}
				break;

				case ParamType::Repeat:
				{
					os << "..." << std::endl;
				}
				break;

				default:
				output_value(*pd.value);
				if(cmd.type_info)
				{
					os << " ";
					output_type_info(pd);
					os << " ";
					output_constraints((*prc.m_provider)[path](prc.m_accessor));
				}
				os << std::endl;
				break;
			}
			return true;
		}

		inline result_type operator() (setvalue_cmd const& cmd)
		{
			auto path = cmd.elem ? prc.m_accessor.find_path(*cmd.elem) : prc.m_accessor.where();
			auto pd = prc.m_accessor.find_param(path);
			// TODO: Perhaps current schema should be stored, maybe within the tree?
			auto sch = (*prc.m_provider)[path](prc.m_accessor);

			bool ok = schema::validate_param(cmd.val, sch);
			if(!ok)
			{
				os << "invalid value" << std::endl;
				return false;
			}

			// TODO: shouldn't need to repeat lookup
			if(cmd.elem)
			{
				prc.m_accessor[*cmd.elem] = cmd.val;
			}
			else
			{
				prc.m_accessor.value_here() = cmd.val;
			}

			// TODO: Overkill
			auto prev_pos = prc.m_accessor.where();
			auto rootname = prc.m_pt.rootname();
			sch = (*prc.m_provider)[rootname](prc.m_accessor);
			prc.m_pt = param_tree::generate_from_schema(sch, prc.m_accessor);
			prc.m_accessor = param_accessor{ &prc.m_pt };
			prc.m_accessor.move_to(prev_pos);

			return true;
		}

		cmdline_processor& prc;
		std::ostream& os;
	};

	std::string cmdline_processor::prompt() const
	{
		return m_accessor.where().to_string();
	}

	bool cmdline_processor::process_cmd(std::string const& cmd_str, std::ostream& out)
	{
		cmd_parser< std::string::const_iterator > parser;
		
		auto it = std::begin(cmd_str);
		command cmd;
		auto result = qi::phrase_parse(it, std::end(cmd_str), parser, qi::space_type{}, cmd);
		//result = result && it == std::end(cmd_str);

		if(!result)
		{
			out << "failed" << std::endl;
			return false;
		}

		cmd_visitor vis{ *this, std::cout };
		return boost::apply_visitor(vis, cmd);
	}

	void cmdline_processor::enter_cmd_loop(std::istream& in, std::ostream& out, std::string const& quit_str)
	{
		while(true)
		{
			out << prompt() << ": ";

			std::string input;
			std::getline(in, input);

			if(input == quit_str)
			{
				break;
			}

			process_cmd(input, out);
		}
	}

}




