// random_par_wgt.cpp

#include "random_par_wgt.h"
#include "pw_yaml.h"
#include "pw_schema_builder.h"

#include "params/param_yaml.h"
#include "params/param_accessor.h"

#include <Wt/WContainerWidget>
#include <Wt/WComboBox>
#include <Wt/WText>
#include <Wt/WHBoxLayout>

#include <boost/variant/get.hpp>


namespace prm
{
	class random_par_wgt::default_impl:
		public param_tree::param_wgt_impl,
		public Wt::WContainerWidget
	{
	public:
		default_impl(YAML::Node schema)
		{
			Wt::WHBoxLayout* layout = new Wt::WHBoxLayout();
			layout->setContentsMargins(0, 0, 0, 0);
			setLayout(layout);

			type_box = new Wt::WComboBox();
			type_box->addItem("Fixed");
			type_box->addItem("Random");
			type_box->setCurrentIndex(0);
			//	type_box->setMinimumSize(75, Wt::WLength::Auto);
			layout->addWidget(type_box);

			double default_val = schema["default"].as< double >();
			double default_min = default_val;
			double default_max = default_val;

			// NOTE: Seems to be bug in YAML double conversions, that results in NAN when converting max double to Node then back to double. Therefore using float max instead.
			
			double min_val = -std::numeric_limits< float >::max();
			if(auto& min = schema["constraints"]["min"])
			{
				min_val = min.as< double >();
				default_min = min_val;
			}
			double max_val = std::numeric_limits< float >::max();
			if(auto& max = schema["constraints"]["max"])
			{
				max_val = max.as< double >();
				default_max = max_val;
			}

			if(auto& def_min = schema["default_min"])
			{
				default_min = def_min.as< double >();
			}
			if(auto& def_max = schema["default_max"])
			{
				default_max = def_max.as< double >();
			}

			fixed_tree = param_tree::create([=](qualified_path const&, param_accessor const&)
			{
				auto s = schema::real("fixed", default_val, min_val, max_val);
				schema::unlabel(s);
				return s;
			});
			layout->addWidget(fixed_tree);

			auto cont = new Wt::WContainerWidget();
			Wt::WHBoxLayout* internal_layout = new Wt::WHBoxLayout();
			internal_layout->setContentsMargins(0, 0, 0, 0);
			cont->setLayout(internal_layout);
			internal_layout->addWidget(new Wt::WText("["));
			random_tree = param_tree::create([=](qualified_path const&, param_accessor const&)
			{
				auto s = schema::list("random");
				schema::layout_horizontal(s);
				schema::unborder(s);

				auto min = schema::real("min", default_min, min_val, max_val);
				schema::unlabel(min);
				schema::append(s, min);

				auto max = schema::real("max", default_max, min_val, max_val);
				schema::unlabel(max);
				schema::append(s, max);

				return s;
			});
			internal_layout->addWidget(random_tree);
			internal_layout->addWidget(new Wt::WText(")"));
			layout->addWidget(cont);

			type_box->changed().connect(std::bind([=]
			{
				auto idx = type_box->currentIndex();
				if(idx == 0)
				{
					// Fixed
					widget(1)->setHidden(false);
					widget(2)->setHidden(true);
				}
				else
				{
					// Random
					widget(1)->setHidden(true);
					widget(2)->setHidden(false);
				}
			}));

			type_box->changed().emit();
		}

	public:
		virtual Wt::WWidget* get_wt_widget()
		{
			return this;
		}

		virtual Wt::Signals::connection connect_handler(pw_event::type type, pw_event_handler const& handler)
		{
			// TODO: !!
			return Wt::Signals::connection();
		}

		virtual param get_locally_instantiated_yaml_param(bool) const
		{
			random result;
			result.is_fixed = type_box->currentIndex() == 0;
			if(result.is_fixed)
			{
				auto param = fixed_tree->get_param_accessor();
				result.range = param["fixed"].as< double >();
			}
			else
			{
				auto param = random_tree->get_param_accessor();
				result.range = std::make_pair(
					param["min"].as< double >(),
					param["max"].as< double >()
					);
			}
			return param{ result };
		}

		virtual bool update_impl_from_yaml_param(param const& p)
		{
			auto v = p.as< random >();
			type_box->setCurrentIndex(v.is_fixed ? 0 : 1);
			widget(1)->setHidden(!v.is_fixed);
			widget(2)->setHidden(v.is_fixed);

			// TODO: This duplication from above schema construction sucks...
			if(v.is_fixed)
			{
				auto val = boost::get< double >(v.range);
				param fixed_p;
				fixed_p[ParamNode::Name] = "fixed";
				fixed_p[ParamNode::Type] = RealNumber;
				fixed_p[ParamNode::Value] = val;
				fixed_tree->set_from_yaml_param(fixed_p);
			}
			else
			{
				auto val = boost::get< std::pair< double, double > >(v.range);
				param random_p;
				random_p[ParamNode::Name] = "random";
				random_p[ParamNode::Type] = List;
				param min;
				min[ParamNode::Name] = "min";
				min[ParamNode::Type] = RealNumber;
				min[ParamNode::Value] = val.first;
				random_p[ParamNode::Value].push_back(min);
				param max;
				max[ParamNode::Name] = "max";
				max[ParamNode::Type] = RealNumber;
				max[ParamNode::Value] = val.second;
				random_p[ParamNode::Value].push_back(max);
				random_tree->set_from_yaml_param(random_p);
			}
			return true;
		}

	private:
		param_tree* fixed_tree;
		param_tree* random_tree;
		Wt::WComboBox* type_box;
	};


	class random_par_wgt::readonly_impl:
		public param_tree::param_wgt_impl,
		public Wt::WText
	{
	public:
		readonly_impl(YAML::Node schema): m_value()
		{
			update_text_from_value();
		}

	public:
		virtual Wt::WWidget* get_wt_widget()
		{
			return this;
		}

		virtual Wt::Signals::connection connect_handler(pw_event::type type, pw_event_handler const& handler)
		{
			// TODO: !!
			return Wt::Signals::connection();
		}

		virtual param get_locally_instantiated_yaml_param(bool) const
		{
			return param{ m_value };
		}

		virtual bool update_impl_from_yaml_param(param const& p)
		{
			m_value = p.as< random >();
			update_text_from_value();
			return true;
		}

	protected:
		void update_text_from_value()
		{
			std::stringstream ss;
			if(m_value.is_fixed)
			{
				ss << boost::get< double >(m_value.range);
			}
			else
			{
				auto range = boost::get< std::pair< double, double > >(m_value.range);
				ss << "[" << range.first << ", " << range.second << ")";
			}

			setText(ss.str());
		}

	private:
		random m_value;
	};


	param_tree::param_wgt_impl* random_par_wgt::create(YAML::Node schema, param_wgt_impl::options_t options)
	{
		bool readonly = schema["readonly"] && schema["readonly"].as< bool >(); //options & param_wgt_impl::ReadOnly != 0;
		if(readonly)
		{
			return new readonly_impl(schema);
		}
		else
		{
			return new default_impl(schema);
		}
	}
}



