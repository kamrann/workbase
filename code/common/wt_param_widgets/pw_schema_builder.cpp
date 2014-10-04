// pw_schema_builder.cpp

#include "pw_schema_builder.h"
#include "container_par_wgt.h"
#include "pw_yaml.h"

#include "params/param_yaml.h"


namespace prm {
	namespace schema {

		void layout_vertical(schema_builder& schema)
		{
			assert(schema["type"].as< ParamType >() == ParamType::List);
			schema["visual"]["layout"] = container_par_wgt::Layout::Vertical;
		}

		void layout_horizontal(schema_builder& schema)
		{
			assert(schema["type"].as< ParamType >() == ParamType::List);
			schema["visual"]["layout"] = container_par_wgt::Layout::Horizontal;
		}
		/*
		void label(schema_builder& sb, std::string const& text)
		{
			sb["visual"]["label"] = text;
		}

		void unlabel(schema_builder& sb)
		{
			sb["visual"].remove("label");
		}

		void border(schema_builder& sb, boost::optional< std::string > label_text)
		{
			sb["visual"]["border"] = true;
			if(label_text)
			{
				label(sb, *label_text);
			}
		}

		void unborder(schema_builder& sb)
		{
			sb["visual"].remove("border");
		}
*/
	}
}


