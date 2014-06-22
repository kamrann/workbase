// random_par_wgt.cpp

#include "random_par_wgt.h"
#include "pw_yaml.h"

#include <Wt/WContainerWidget>
#include <Wt/WComboBox>
#include <Wt/WText>
#include <Wt/WHBoxLayout>

#include <boost/variant/get.hpp>


namespace prm
{
	class random_par_wgt::impl: public Wt::WContainerWidget
	{
	public:
		impl(YAML::Node const& script)
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

			double default_val = script["default"].as< double >();
			double default_min = default_val;
			double default_max = default_val;

			// NOTE: Seems to be bug in YAML double conversions, that results in NAN when converting max double to Node then back to double. Therefore using float max instead.
			
			double min_val = -std::numeric_limits< float >::max();
			if(auto& min = script["constraints"]["min"])
			{
				min_val = min.as< double >();
				default_min = min_val;
			}
			double max_val = std::numeric_limits< float >::max();
			if(auto& max = script["constraints"]["max"])
			{
				max_val = max.as< double >();
				default_max = max_val;
			}

			if(auto& def_min = script["default_min"])
			{
				default_min = def_min.as< double >();
			}
			if(auto& def_max = script["default_max"])
			{
				default_max = def_max.as< double >();
			}

			fixed_tree = param_tree::create([=](YAML::Node const&)
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
			random_tree = param_tree::create([=](YAML::Node const&)
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
		random get_value() const
		{
			random result;
			result.is_fixed = type_box->currentIndex() == 0;
			if(result.is_fixed)
			{
				result.range = fixed_tree->get_yaml_param().as< double >();
			}
			else
			{
				auto param = random_tree->get_yaml_param();
				auto str = YAML::Dump(param);
				result.range = std::make_pair(
					find_value(param, "min").as< double >(),
					find_value(param, "max").as< double >()
					);
			}
			return result;
		}

		void set(random const& v)
		{
			// TODO:
/*			type_box->setCurrentIndex(v.is_fixed ? 0 : 1);
			if(v.is_fixed)
			{
				fixed_tree->set_
			}
			else
			{

			}
			*/
		}

	private:
		param_tree* fixed_tree;
		param_tree* random_tree;
		Wt::WComboBox* type_box;
	};

	Wt::WWidget* random_par_wgt::create_impl(YAML::Node const& script)
	{
		m_impl = new impl(script);
		return m_impl;
	}

	param random_par_wgt::get_param() const
	{
		return m_impl->get_value();
	}

	void random_par_wgt::set_from_param(param const& p)
	{
		m_impl->set(boost::get< random >(p));
	}
}



