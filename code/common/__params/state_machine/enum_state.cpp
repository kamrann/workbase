// enum_state.cpp

#include "enum_state.h"

namespace prm {
	namespace fsm {

		void enumeration::on_add(clsm::ev_cmd< enum_add_cmd > const& cmd)
		{
			auto& editor = context< paramtree_editor >();
			auto acc = editor.acc;

			qualified_path path = acc.where();
			auto& pd = acc.find_param(path);
			// TODO: Perhaps current schema should be stored, maybe within the tree?
			auto sch = (*editor.provider)[path.unindexed()](acc);

			enum_param_val to_add{ std::begin(cmd.elements), std::end(cmd.elements) };

			// TODO: Ideally want to call a method to validate individual elements in isolation
			bool ok = schema::validate_param(to_add, sch);
			if(!ok)
			{
				editor.output("invalid values");
				return;// false;
			}

			// TODO: shouldn't need to repeat lookup
			auto epv = extract_as< enum_param_val >(*pd.value);
			for(auto it = std::begin(to_add); it != std::end(to_add);)
			{
				auto existing_it = std::find(std::begin(epv), std::end(epv), *it);
				if(existing_it == std::end(epv))
				{
					// Add the element, and leave it in to_add list
					epv.push_back(*it);
					++it;
				}
				else
				{
					it = to_add.erase(it);
				}
			}

			if(to_add.empty())
			{
				// No elements were added
				editor.output("nothing added");
			}
			else
			{
				pd.value = epv;
				// TODO: Overkill
				editor.reload_pt_schema();

				std::stringstream ss;
				ss << "added: { ";
				for(auto it = std::begin(to_add); it != std::end(to_add); ++it)
				{
					ss << *it;
					if(it != std::prev(std::end(to_add)))
					{
						ss << ", ";
					}
				}
				ss << " }";
				editor.output(ss.str());
			}
		}

		void enumeration::on_remove(clsm::ev_cmd< enum_remove_cmd > const& cmd)
		{
			auto& editor = context< paramtree_editor >();
			auto acc = editor.acc;

			qualified_path path = acc.where();
			auto& pd = acc.find_param(path);
			// TODO: Perhaps current schema should be stored, maybe within the tree?
			auto sch = (*editor.provider)[path.unindexed()](acc);

			enum_param_val to_remove{ std::begin(cmd.elements), std::end(cmd.elements) };

			// TODO: Ideally want to call a method to validate individual elements in isolation
			bool ok = schema::validate_param(to_remove, sch);
			if(!ok)
			{
				editor.output("invalid values");
				return;// false;
			}

			// TODO: shouldn't need to repeat lookup
			auto epv = extract_as< enum_param_val >(*pd.value);
			for(auto it = std::begin(to_remove); it != std::end(to_remove);)
			{
				auto existing_it = std::find(std::begin(epv), std::end(epv), *it);
				if(existing_it == std::end(epv))
				{
					it = to_remove.erase(it);
				}
				else
				{
					// Remove the element, and leave it in to_remove list
					epv.erase(existing_it);
					++it;
				}
			}

			if(to_remove.empty())
			{
				// No elements were removed
				editor.output("nothing removed");
			}
			else
			{
				pd.value = epv;
				// TODO: Overkill
				editor.reload_pt_schema();

				std::stringstream ss;
				ss << "removed { ";
				for(auto it = std::begin(to_remove); it != std::end(to_remove); ++it)
				{
					ss << *it;
					if(it != std::prev(std::end(to_remove)))
					{
						ss << ", ";
					}
				}
				ss << " }";
				editor.output(ss.str());
			}
		}

		std::string enumeration::get_prompt() const
		{
			auto location = context< paramtree_editor >().acc.where();
			return location.to_string() + " [enum]: ";
		}

	}
}




