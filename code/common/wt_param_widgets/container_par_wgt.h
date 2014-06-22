// container_par_wgt.h

#ifndef __WB_CONTAINER_PARAM_WIDGET_H
#define __WB_CONTAINER_PARAM_WIDGET_H

#include "par_wgt.h"


namespace prm
{
	class container_par_wgt:
		public param_tree::param_wgt
	{
	public:
		enum Layout {
			Vertical,
			Horizontal,

			Count,
		};

	public:
		container_par_wgt(schema_accessor_fn& schema_accessor);
		virtual param get_param() const;
		virtual YAML::Node get_yaml_param() const;
		virtual void set_from_param(param const& p);

		void add_child(param_wgt* w);
		void clear_children(size_t from_index = 0);

	protected:
		virtual Wt::WWidget* create_impl(YAML::Node const& script);// , schema_accessor_fn& schema_accessor);

		void container_par_wgt::create_child_widgets(
			//prm::container_par_wgt* parent,
			std::string const& schema_name,
			YAML::Node const& schema,
			//schema_accessor_fn& schema_accessor,
			size_t start_index = 0);

	protected:
		class impl;
		impl* m_impl;

		schema_accessor_fn& m_schema_accessor;
	};
}


#endif


