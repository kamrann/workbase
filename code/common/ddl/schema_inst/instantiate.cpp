// instantiate.cpp

#include "instantiate.h"
#include "../values/navigator.h"
#include "../ddl.h"	// TODO: overkill


namespace ddl {

	boolean_sch_node instantiate(boolean_defn_node const& nd, navigator nav, ref_resolver const& rr)
	{
		boolean_sch_node sch;
		sch.default(nd.default());
		return sch;
	}

	integer_sch_node instantiate(integer_defn_node const& nd, navigator nav, ref_resolver const& rr)
	{
		integer_sch_node sch;
		sch.default(nd.default());
		sch.min(nd.min());
		sch.max(nd.max());
		return sch;
	}

	realnum_sch_node instantiate(realnum_defn_node const& nd, navigator nav, ref_resolver const& rr)
	{
		realnum_sch_node sch;
		sch.default(nd.default());
		sch.min(nd.min());
		sch.max(nd.max());
		return sch;
	}

	string_sch_node instantiate(string_defn_node const& nd, navigator nav, ref_resolver const& rr)
	{
		string_sch_node sch;
		sch.default(nd.default());
		sch.minlength(nd.minlength());
		sch.maxlength(nd.maxlength());
		sch.charset(nd.charset());
		return sch;
	}

	enum_sch_node instantiate(enum_defn_node const& nd, navigator nav, ref_resolver const& rr)
	{
		enum_sch_node sch;
		sch.enum_values(nd.enum_values(nav));
		sch.minsel(nd.minsel());
		sch.maxsel(nd.maxsel());
		sch.default(nd.default());
		return sch;
	}

	list_sch_node instantiate(list_defn_node const& nd, navigator nav, ref_resolver const& rr)
	{
		list_sch_node sch;
		std::vector< sch_node > entries;
		if(nav && nav.is_active())
		{
			auto entrydefn = nd.entrydefn();
			// TODO: is it okay to assume that if the nav is active, then we are positioned at a list node??
			// guess not
			if(nav.get().is_list())
			{
				// Another thing: wouldn't it make sense to have the restriction that schema instantiation cannot be
				// dependent upon any values which are descendents in the hierarchy of the node to be instantiated??
				auto count = nav.list_num_items();
				for(size_t idx = 0; idx < count; ++idx)
				{
					auto item = instantiate(entrydefn, nav[idx], rr);
					if(item)
					{
						entries.push_back(item);
					}
				}
			}
		}
		sch.entries(std::move(entries));
		sch.minentries(nd.minentries());
		sch.maxentries(nd.maxentries());
		return sch;
	}

	composite_sch_node instantiate(composite_defn_node const& nd, navigator nav, ref_resolver const& rr)
	{
		composite_sch_node sch;
		auto child_defns = nd.children();
		for(auto const& cd : child_defns)
		{
			auto defn = cd.second;
			auto sn = instantiate(defn, (nav && nav.is_active() && nav.is_child(cd.first)) ? nav[cd.first] : nav.deactivated(), rr);
			if(sn)
			{
				sch.add(cd.first, std::move(sn));
			}
		}
		return sch;
	}

	sch_node instantiate(conditional_defn_node const& nd, navigator nav, ref_resolver const& rr)
	{
		// Evaluate condition
		/*
		TODO: Is it always safe to let the resolver try to resolve references even with a deactivated nav?
		Point is, we know only that the nav is not at the current location; we have no idea where it is.
		Couldn't this lead to reference resolutions that we didn't expect?
		*/
		auto defn = nd.evaluate(rr, nav);
		// And instantiate resulting definition
		return instantiate(defn, nav, rr);
	}

	struct instantiation_visitor:
		public boost::static_visitor< sch_node >
	{
		instantiation_visitor(navigator nav, ref_resolver const& rr):
			nav_{ nav },
			rr_{ rr }
		{}

		template < typename DefnNode >
		inline result_type operator() (DefnNode const& nd) const
		{
			return instantiate(nd, nav_, rr_);
		}

		navigator nav_;
		ref_resolver const& rr_;
	};

	sch_node instantiate(defn_node const& defn, navigator nav, ref_resolver const& rr)//value_node const& val)
	{
		if(!defn)
		{
			return{};
		}

		auto vis = instantiation_visitor{ nav, rr };// val
		auto nd = defn.apply_visitor(vis);
		nd.set_id(defn.get_id());
		return nd;
	}

}





