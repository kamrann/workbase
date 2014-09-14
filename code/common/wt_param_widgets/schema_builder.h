// schema_builder.h

#ifndef __WB_SCHEMA_BUILDER_H
#define __WB_SCHEMA_BUILDER_H

#include "pw_schema.h"

#include <boost/optional.hpp>


namespace prm {
	namespace schema {

		typedef schema_node schema_builder;

		schema_builder boolean(std::string const& name, bool default_val);
		schema_builder integer(std::string const& name, int default_val, boost::optional< int > min_val = boost::optional< int >(), boost::optional< int > max_val = boost::optional< int >());
		schema_builder real(std::string const& name, double default_val, boost::optional< double > min_val = boost::optional< double >(), boost::optional< double > max_val = boost::optional< double >());
		schema_builder string(std::string const& name, std::string const& default_val = "");
		schema_builder enum_selection(std::string const& name, std::vector< std::pair< std::string, std::string > > const& values, size_t min_sel = 1, size_t max_sel = 1);
		schema_builder enum_selection(std::string const& name, std::vector< std::string > const& labels, size_t min_sel = 1, size_t max_sel = 1);
		schema_builder random(std::string const& name, double default_val, boost::optional< double > min_val, boost::optional< double > max_val, boost::optional< double > default_min = boost::none, boost::optional< double > default_max = boost::none);
		schema_builder list(std::string const& name);
		schema_builder repeating_list(std::string const& name, std::string const& contents, size_t min_count = 0, size_t max_count = 0);

		void append(schema_builder& schema, schema_builder const& child);
		void layout_vertical(schema_builder& schema);
		void layout_horizontal(schema_builder& schema);
		void enable_import(schema_builder& schema, std::string const& filter);

		void label(schema_builder& sb, std::string const& text);
		void unlabel(schema_builder& sb);
		void border(schema_builder& sb, boost::optional< std::string > label_text = boost::none);
		void unborder(schema_builder& sb);
		void readonly(schema_builder& sb, bool ro = true);

		void trigger(schema_builder& sb, /* todo: event specifier, */ std::string const& event_name);
		void update_on(schema_builder& sb, std::string const& event_name);	//std::string wgt_id);

		schema_builder last(schema_builder& sb);

	}
}



#endif


