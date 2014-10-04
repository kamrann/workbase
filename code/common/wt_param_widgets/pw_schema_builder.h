// pw_schema_builder.h

#ifndef __WB_PARAM_WIDGET_SCHEMA_BUILDER_H
#define __WB_PARAM_WIDGET_SCHEMA_BUILDER_H

#include "params/schema_builder.h"

#include <boost/optional.hpp>


namespace prm {
	namespace schema {

		void layout_vertical(schema_builder& schema);
		void layout_horizontal(schema_builder& schema);

/*		void label(schema_builder& sb, std::string const& text);
		void unlabel(schema_builder& sb);
		void border(schema_builder& sb, boost::optional< std::string > label_text = boost::none);
		void unborder(schema_builder& sb);
		*/
		//		void readonly(schema_builder& sb, bool ro = true);

	}
}



#endif


