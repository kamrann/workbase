// db_params.h

#ifndef __DB_PARAMS_H
#define __DB_PARAMS_H

#include "wt_param_widgets/par_wgt.h"
#include "wt_param_widgets/param_manager.h"

#include <Wt/WWidget>
//
#include <Wt/WContainerWidget>
//
#include <Wt/WGroupBox>
#include <Wt/Dbo/Dbo>

#include <boost/variant/variant.hpp>
#include <boost/variant/static_visitor.hpp>
#include <boost/variant/get.hpp>
#include <boost/mpl/vector.hpp>
#include <boost/mpl/for_each.hpp>

#include <vector>
#include <map>
#include <set>


namespace dbo = Wt::Dbo;
namespace mpl = boost::mpl;


// This may need to be polymorphic class like existing param_type...
enum ParamType {
	// Standard types
	Boolean,
	Integer,
	RealNumber,
	String,
	Enumeration,
	Vector2,
//	Vector3,

	// Nested types
//	SubType,
	ChildType,

	Count,
	Invalid = Count,
};

//class hierarchy_object;

struct param_spec
{
	std::string				name;
	ParamType				type;			// TODO: really should combine type & constraints into one
		// structure, to ensure they are both of corresponding param type
	prm::par_constraints	constraints;
	// TODO: should really be part of constraints, since we can't have both...
//	hierarchy_object*		child;

	param_spec(
		ParamType _t = ParamType::Invalid,
		prm::par_constraints const& _c = prm::par_null_constraints(),
		std::string _n = ""): name(_n), constraints(_c), type(_t)//, child(nullptr)
	{}
};

typedef std::vector< param_spec > param_spec_list;

typedef std::map< std::string, prm::param > param_map;


/*
DB Hierarchy Level Definition
*/
struct level_defn
{
	struct no_parent {};	// If this has no parent type

	template < typename T >
	struct is_a {};			// If this a a subtype of T

	template < typename T >
	struct is_child_of {};	// If this is a child of T

	typedef no_parent parent_t;

	// TODO: Perhaps store as enums instead, and associate each definition with an enum value.
	// Then we can make use of the enums within code, and substitute with strings for database storage.
	// Ideally rather than a single big enum, there would be an enum at defn level, with a value for each of its
	// subtype definitions.
	// See test code in Cat defn
	typedef mpl::vector<> children_t;
	typedef mpl::vector<> subtypes_t;

//	level_name() static method
//	get_specs() static method
};


template < typename T >
struct test_is_a: public mpl::false_
{};

template < typename Defn >
struct test_is_a< level_defn::is_a< Defn > >: public mpl::true_
{};


template < typename >
class db_hierarchy_level;


template < typename ParentDefn >
struct record_with_parent
{
	typedef ParentDefn parent_defn_t;
	typedef typename db_hierarchy_level< parent_defn_t >::record parent_record_t;
	typedef dbo::ptr< parent_record_t > parent_ptr_t;

	parent_ptr_t parent_ptr;
};

template < typename ParentDefn >
struct child_parent_data: public record_with_parent< ParentDefn >
{
	template < class Action >
	inline void persist(Action& a)
	{
		// Parent/child relationship - Have foreign key to parent separate from our own id
		dbo::belongsTo(a, parent_ptr, parent_defn_t::level_name(), dbo::NotNull | dbo::OnDeleteCascade);
	}
};

template < typename ParentDefn >
struct subtype_parent_data: public record_with_parent< ParentDefn >
{
	template < class Action >
	inline void persist(Action& a)
	{
		// Supertype/subtype relationship - Use same column as both promary and foreign key
		dbo::id(a, parent_ptr, parent_defn_t::level_name(), dbo::NotNull | dbo::OnDeleteCascade);
	}
};

struct record_without_parent
{
	template < class Action >
	inline void persist(Action& a)
	{}

	struct parent_ptr_t {} parent_ptr;
};


template < typename Parent >
struct record_parent_data;

template < typename ParentDefn >
struct record_parent_data< level_defn::is_child_of< ParentDefn > >: public child_parent_data< ParentDefn >
{};

template < typename ParentDefn >
struct record_parent_data< level_defn::is_a< ParentDefn > >: public subtype_parent_data< ParentDefn >
{};

template <>
struct record_parent_data< level_defn::no_parent >: public record_without_parent
{};


// TODO: Perhaps instead of storing type, could store a variant over dbo::ptrs to all possible subtype records
// This way we can have complete access to the hierarchy through a single ptr to a top level object, and don't need to mess
// around with joins.
template < typename SubtypeDefns >
struct record_subtype_data
{
	template < class Action >
	inline void persist(Action& a)
	{
		dbo::field(a, st_id_string, "subtype");// TODO: defn_t::subtype_name());
	}

	std::string st_id_string;
};

template <>
struct record_subtype_data< mpl::vector<> >
{
	template < class Action >
	inline void persist(Action& a)
	{}
};


template < typename Defn >
struct record_base
{
	typedef Defn defn_t;

	prm::param_list		fields;

	record_base()
	{
		param_spec_list psl = defn_t::get_specs();
		for(param_spec const& s : psl)
		{
			if(s.type != ChildType)
			{
				fields.push_back(prm::create_from_type((prm::ParamType)s.type));
			}
		}
	}

	template < class Action >
	struct persist_visitor: public boost::static_visitor< void >
	{
		persist_visitor(Action& a, std::string const& n): action(a), name(n)
		{}

		inline void operator() (bool& val)
		{
			dbo::field(action, val, name);
		}

		inline void operator() (int& val)
		{
			dbo::field(action, val, name);
		}

		inline void operator() (double& val)
		{
			dbo::field(action, val, name);
		}

		inline void operator() (std::string& val)
		{
			dbo::field(action, val, name);
		}

		inline void operator() (prm::enum_param_val& val)
		{
			// TODO: ??? dbo::field(action, val, name);
		}

		inline void operator() (prm::vec2& val)
		{
			// TODO: Better to overload field() for vector type, probably
			dbo::field(action, val[0], name + "_x");
			dbo::field(action, val[1], name + "_y");
		}

		inline void operator() (prm::random& val)
		{
			// TODO:
		}

		inline void operator() (std::vector< prm::param >& val)
		{
			// TODO: ??? dbo::field(action, val, name);
		}

		Action& action;
		std::string const& name;
	};

	template < class Action >
	inline void persist(Action& a)
	{
		param_spec_list psl = defn_t::get_specs();
		for(size_t i = 0; i < fields.size(); ++i)
		{
			if(psl[i].type != ChildType)
			{
				persist_visitor< Action > vis(a, psl[i].name);
				boost::apply_visitor(vis, fields[i]);
			}
		}
	}
};

template < typename Typelist, typename Ftr >
inline void for_each_type(/*Typelist,*/ Ftr ftr)
{
	mpl::for_each< Typelist, mpl::make_identity<mpl::_> >(ftr);
}

template < typename Ftr >
struct nth_type_ftr
{
	nth_type_ftr(size_t _n, Ftr _ftr): n(_n), i(0), ftr(_ftr)
	{}

	template < typename Defn >
	inline void operator() (mpl::identity< Defn >)
	{
		if(i++ == n)
		{
			result = ftr(mpl::identity< Defn >());
		}
	}

	size_t n, i;
	Ftr ftr;
	typename Ftr::result_t result;
};

template < typename Typelist, typename Ftr >
inline typename Ftr::result_t for_nth_type(/*Typelist,*/ size_t n, Ftr& ftr)
{
	nth_type_ftr< Ftr > nth_ftr(n, ftr);
	for_each_type< Typelist >(std::ref(nth_ftr));
	return nth_ftr.result;
}


template < typename RecordImpl >
struct surrogate_pk_record: public RecordImpl
{};

template < typename RecordImpl >
struct natural_pk_record: public RecordImpl
{};


template < typename Defn >
class db_hierarchy_level
{
protected:
	typedef Defn defn_t;
	typedef typename defn_t::parent_t parent_t;
	typedef typename defn_t::subtypes_t subtype_defns_t;
	typedef typename defn_t::children_t child_defns_t;

public:
	struct record_impl:
		public record_base< defn_t >,
		public record_parent_data< parent_t >,
		public record_subtype_data< subtype_defns_t >
	{
		typedef record_base< defn_t > rec_base_t;
		typedef record_parent_data< parent_t > rec_parent_t;
		typedef record_subtype_data< subtype_defns_t > rec_subtype_t;

		template < class Action >
		inline void persist(Action& a)
		{
			rec_base_t::persist(a);
			rec_parent_t::persist(a);
			rec_subtype_t::persist(a);
		}
	};

	typedef typename mpl::if_<
		test_is_a< parent_t >,
		natural_pk_record< record_impl >,
		surrogate_pk_record< record_impl >
	>::type record;

public:
	static Wt::WWidget* create_widget(prm::param_mgr& mgr)
	{
		Wt::WContainerWidget* cont = new Wt::WContainerWidget();
		param_spec_list specs = defn_t::get_specs();
		for(param_spec spec : specs)
		{
			switch(spec.type)
			{
				case ParamType::ChildType:
				{
					/*					Wt::WWidget* child_w = spec.child->create_widget(mgr);
					Wt::WGroupBox* group = new Wt::WGroupBox(spec.name);
					group->addWidget(child_w);
					cont->addWidget(group);
					*/				
				}
				break;

				default:
				{
					prm::param_wgt* pw = prm::param_wgt::create((prm::ParamType)spec.type, spec.constraints, spec.name);
					// TODO: 
					prm::param_wgt_id pw_id = spec.name;
					//
					mgr.register_widget(pw_id, pw);
					cont->addWidget(pw);
				}
				break;
			}
		}

		create_all_child_widgets(cont, mgr);

		create_subtype_widgets< subtype_defns_t >(cont, mgr);

		return cont;
	}

	static bool map_database_table(dbo::Session& session)
	{
		session.mapClass< record >(defn_t::level_name());
		return true;
	}

	static dbo::ptr< record > add_record_from_input(dbo::Session& session, prm::param_mgr const& mgr, typename record::parent_ptr_t parent = record::parent_ptr_t())
	{
		// Construct our own record, setting parent pointer if we have one (should be passed in)
		dbo::ptr< record > rec = new record;
		rec.modify()->fields = construct_param_list(mgr);
		rec.modify()->parent_ptr = parent;
		store_subtype_id< subtype_defns_t >(rec, mgr);
		session.add(rec);

		add_all_child_records(session, mgr, rec);

		add_subtype_record< subtype_defns_t >(session, mgr, rec);

		return rec;
	}

	static dbo::ptr< record > retrieve_record(dbo::Session& session, typename dbo::dbo_traits< record >::IdType& id)
	{
		dbo::ptr< record > rec = session.load< record >(id);
		return rec;
	}

	static param_map retrieve_params(dbo::Session& session, typename dbo::dbo_traits< record >::IdType& id)
	{
		param_map pm;
		dbo::ptr< record > rec = retrieve_record(session, id);
		
		// Store our fields into pm
		param_spec_list psl = defn_t::get_specs();
		size_t idx = 0;
		for(param_spec const& spec : specs)
		{
			pm[spec.name] = rec->fields[idx++];
		}
		
		retrieve_subtype_params(session, rec, pm);
/*		if(subtypes)
		{
			use functor with our subtype id_string field to recursively call retrieve_params(session, rec) on correct subtype;
		}
*/
		return pm;
	}

protected:
	struct get_level_names_ftr
	{
		std::vector< std::string >& names;

		get_level_names_ftr(std::vector< std::string >& _names): names(_names)
		{}

		template < typename Defn >
		inline void operator() (mpl::identity< Defn >)
		{
			names.push_back(Defn::level_name());
		}
	};

	struct create_widget_ftr
	{
		create_widget_ftr(prm::param_mgr& _mgr): mgr(_mgr)
		{}

		typedef Wt::WWidget* result_t;

		template < typename Defn >
		inline result_t operator() (mpl::identity< Defn >) const
		{
			return db_hierarchy_level< Defn >::create_widget(mgr);
		}

		prm::param_mgr& mgr;
	};

	struct create_child_ftr
	{
		create_child_ftr(Wt::WContainerWidget* _cont, prm::param_mgr& _mgr): cont(_cont), mgr(_mgr)
		{}

		template < typename Defn >
		inline void operator() (mpl::identity< Defn > dummy) const
		{
			Wt::WWidget* child_w = create_widget_ftr(mgr)(dummy);
			// TODO: mgr.register???
			Wt::WGroupBox* group = new Wt::WGroupBox(Defn::level_name());
			group->addWidget(child_w);
			cont->addWidget(group);
		}

		Wt::WContainerWidget* cont;
		prm::param_mgr& mgr;
	};

	static void create_all_child_widgets(Wt::WContainerWidget* cont, prm::param_mgr& mgr)
	{
		for_each_type< child_defns_t >(create_child_ftr(cont, mgr));
	}

	template < typename Subtypes, typename boost::enable_if< mpl::not_< mpl::empty< Subtypes > >, int >::type = 0 >
	static void create_subtype_widgets(Wt::WContainerWidget* cont, prm::param_mgr& mgr)
	{
		std::vector< std::string > st_names;
		for_each_type< subtype_defns_t >(get_level_names_ftr(st_names));
		prm::enum_par_constraints enum_data;
		size_t idx = 0;
		for(std::string & name : st_names)
		{
			enum_data.add(name, boost::any(idx));
			++idx;
		}
		prm::param_wgt* subtype_sel_w = prm::param_wgt::create(prm::ParamType::Enumeration, enum_data, defn_t::subtype_name());
		// TODO:
		prm::param_wgt_id pw_id = defn_t::subtype_name();
		//
		mgr.register_widget(pw_id, subtype_sel_w);
		cont->addWidget(subtype_sel_w);

		size_t stw_index = cont->count();
		cont->addWidget(new Wt::WContainerWidget);	// Placeholder

		boost::function< void() > handler = [&mgr, cont, stw_index, pw_id](){
			cont->removeWidget(cont->widget(stw_index));
			prm::param subtype_param = mgr.retrieve_param(pw_id);
			prm::enum_param_val subtype = boost::get< prm::enum_param_val >(subtype_param);
			size_t st_index = boost::any_cast<size_t>(subtype.contents);
			create_widget_ftr cw_ftr(mgr);
			Wt::WWidget* subtype_w = for_nth_type< subtype_defns_t >(st_index, cw_ftr);

			/* TODO: prm::param_wgt* */ //Wt::WWidget* subtype_w = sub_hierarchy->create_widget(mgr);
			/*/ TODO:
			prm::param_wgt_id pw_id = subtype_name();
			//
			mgr.register_widget(pw_id, subtype_sel_w);
			*/
			cont->insertWidget(stw_index, subtype_w);
		};
		handler();

		mgr.register_change_handler(pw_id, handler);
	}

	template < typename Subtypes, typename boost::enable_if< mpl::empty< Subtypes >, int >::type = 0 >
	static void create_subtype_widgets(Wt::WContainerWidget* cont, prm::param_mgr& mgr)
	{
		// If no subtypes, do nothing
	}

	template < typename Subtypes, typename boost::enable_if< mpl::not_< mpl::empty< Subtypes > >, int >::type = 0 >
	static void store_subtype_id(dbo::ptr< record > rec, prm::param_mgr const& mgr)
	{
		// TODO: ids not names
		std::vector< std::string > names;
		for_each_type< Subtypes >(get_level_names_ftr(names));

		prm::enum_param_val st_param = boost::get< prm::enum_param_val >(mgr.retrieve_param(defn_t::subtype_name()));
		size_t st_idx = boost::any_cast<size_t>(st_param.contents);
		rec.modify()->st_id_string = names[st_idx];
	}

	template < typename Subtypes, typename boost::enable_if< mpl::empty< Subtypes >, int >::type = 0 >
	static void store_subtype_id(dbo::ptr< record > rec, prm::param_mgr const& mgr)
	{}

	struct add_record_functor
	{
		add_record_functor(
			dbo::Session& _session,
			prm::param_mgr const& _mgr,
			dbo::ptr< record > _rec_ptr
			):
			session(_session),
			mgr(_mgr),
			rec_ptr(_rec_ptr)
		{}

		/// TODO: see below
		typedef int result_t;

		template < class T >
		result_t operator() (mpl::identity< T >)
		{
			// TODO: To enable this to return correct ptr<> type, will need to make ftr itself the template
			// (as opposed to just this operator) and then use a template template param to allow ftr of correct type to
			// be constructed for each type in typelist.
			db_hierarchy_level< T >::add_record_from_input(session, mgr, rec_ptr);
			return 0;
		}

		dbo::Session& session;
		prm::param_mgr const& mgr;
		dbo::ptr< record > rec_ptr;
	};

	static void add_all_child_records(dbo::Session& session, prm::param_mgr const& mgr, dbo::ptr< record > rec)
	{
		add_record_functor ftr(session, mgr, rec);
		for_each_type< child_defns_t >(ftr);
	}

	static prm::param_list construct_param_list(prm::param_mgr const& mgr)
	{
		param_spec_list psl = defn_t::get_specs();
		prm::param_list params;
		for(param_spec const& s : psl)
		{
			if(s.type != ChildType)
			{
				params.push_back(mgr.retrieve_param(s.name));
			}
		}

		return params;
	}

	template < typename Subtypes, typename boost::enable_if< mpl::not_< mpl::empty< Subtypes > >, int >::type = 0 >
	static void add_subtype_record(dbo::Session& session, prm::param_mgr const& mgr, dbo::ptr< record > rec)
	{
		prm::enum_param_val st_param = boost::get< prm::enum_param_val >(mgr.retrieve_param(defn_t::subtype_name()));
		size_t st_idx = boost::any_cast<size_t>(st_param.contents);

		for_nth_type< subtype_defns_t >(st_idx, add_record_functor(session, mgr, rec));
	}

	template < typename Subtypes, typename boost::enable_if< mpl::empty< Subtypes >, int >::type = 0 >
	static void add_subtype_record(dbo::Session& session, prm::param_mgr const& mgr, dbo::ptr< record > rec)
	{
		// No subtypes, do nothing
	}

	struct get_subtype_params_ftr
	{
		get_subtype_params_ftr(dbo::Session& _session, dbo::ptr< record > _rec): session(_session), rec(_rec)
		{}

		typedef param_map result_t;

		template < typename T >
		inline result_t operator() (mpl::identity< T >)
		{
			return db_hierarchy_level< T >::retrieve_params(session, rec);
		}

		dbo::Session& session;
		dbo::ptr< record > rec;
	};

	template < typename Subtypes, typename boost::enable_if< mpl::not_< mpl::empty< Subtypes > >, int >::type = 0 >
	static void retrieve_subtype_params(dbo::Session& session, dbo::ptr< record > rec, param_map& pm)
	{
		// TODO: ids not names
		std::vector< std::string > names;
		for_each_type< Subtypes >(get_level_names_ftr(names));

		size_t idx = std::find(names.begin(), names.end(), rec->st_id_string) - names.begin();
		param_map st_pm = for_nth_type< Subtypes >(idx, get_subtype_params_ftr(session, rec));
		for(auto pr : st_pm)
		{
			pm[pr.first] = pr.second;
		}
	}

	template < typename Subtypes, typename boost::enable_if< mpl::empty< Subtypes >, int >::type = 0 >
	static void retrieve_subtype_params(dbo::Session& session, dbo::ptr< record > rec, param_map& pm)
	{
		// No subtypes, do nothing
	}
};


namespace Wt {
	namespace Dbo {
		
		template < typename RecordImpl >
		struct dbo_traits< natural_pk_record< RecordImpl > >: public dbo_default_traits
		{
			typedef typename RecordImpl::parent_ptr_t IdType;

			static IdType invalidId()
			{
				return IdType();
			}

			static const char* surrogateIdField()
			{
				return 0;
			}
		};

	}
}


class Dog;
class Cat;
class House;

class Animal: public level_defn
{
public:
	typedef mpl::vector<
		Dog,
		Cat
	>::type subtypes_t;

	typedef mpl::vector<
		House
	>::type children_t;

	static inline const char* level_name()
	{
		return "Animal";
	}

	static inline const char* subtype_name()
	{
		return "Family";
	}

	static param_spec_list get_specs();
};

class House: public level_defn
{
public:
	typedef level_defn::is_child_of< Animal > parent_t;

	static inline const char* level_name()
	{
		return "House";
	}

	static param_spec_list get_specs()
	{
		param_spec_list sl;
		sl.push_back(param_spec(ParamType::String, prm::string_par_constraints(), "Location"));
		return sl;
	}
};

class Dog: public level_defn
{
public:
	typedef level_defn::is_a< Animal > parent_t;

	static inline const char* level_name()
	{
		return "Dog";
	}

	static param_spec_list get_specs()
	{
		param_spec_list sl;
		sl.push_back(param_spec(ParamType::RealNumber, prm::realnum_par_constraints(), "Stick Enthusiasm"));
		return sl;
	}
};

class DomesticCat;
class Tiger;

//
#include <boost/mpl/map.hpp>
//

class Cat: public level_defn
{
public:
	typedef level_defn::is_a< Animal > parent_t;

	/* Test * /
	enum _Subtypes {
		_DomesticCat,
		_Tiger,

		Count,
	};

	typedef mpl::map <
		mpl::pair< mpl::int_< _DomesticCat >, DomesticCat >
		, mpl::pair< mpl::int_< _Tiger >, Tiger >
	>::type _subtypes_t;
	/**/

	typedef mpl::vector<
		DomesticCat,
		Tiger
	>::type subtypes_t;

	static inline const char* level_name()
	{
		return "Cat";
	}

	static inline const char* subtype_name()
	{
		return "Species";
	}

	static param_spec_list get_specs()
	{
		param_spec_list sl;
		sl.push_back(param_spec(ParamType::Integer, prm::integer_par_constraints(), "Num Whiskers"));
		return sl;
	}
};

class DomesticCat: public level_defn
{
public:
	typedef level_defn::is_a< Cat > parent_t;

	static inline const char* level_name()
	{
		return "Domestic Cat";
	}

	static param_spec_list get_specs()
	{
		param_spec_list sl;
		sl.push_back(param_spec(ParamType::Boolean, prm::boolean_par_constraints(), "Chases Mice"));
		return sl;
	}
};

class Tiger: public level_defn
{
public:
	typedef level_defn::is_a< Cat > parent_t;
	
	static inline const char* level_name()
	{
		return "Tiger";
	}

	static param_spec_list get_specs()
	{
		param_spec_list sl;
		sl.push_back(param_spec(ParamType::Boolean, prm::boolean_par_constraints(), "In Zoo"));
		sl.push_back(param_spec(ParamType::Integer, prm::integer_par_constraints(), "Num Stripes"));
		return sl;
	}
};



#endif


