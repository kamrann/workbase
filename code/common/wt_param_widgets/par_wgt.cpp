// par_wgt.cpp

#include "par_wgt.h"

#include "boolean_par_wgt.h"
#include "integer_par_wgt.h"
#include "realnum_par_wgt.h"
#include "string_par_wgt.h"
#include "enum_par_wgt.h"
#include "vector_par_wgt.h"

#include "container_par_wgt.h"

#include <Wt/WText>
#include <Wt/WContainerWidget>
#include <Wt/WFormWidget>


namespace prm
{
	void param_wgt::initialize(pw_options const& opt, std::string const& label)
	{
		Wt::WText* label_w = new Wt::WText(label);
		m_base_impl = create_impl(opt);
		Wt::WContainerWidget* cont = new Wt::WContainerWidget();
		cont->addWidget(label_w);
		cont->addWidget(m_base_impl);
		setImplementation(cont);
	}

	param_wgt* param_wgt::create(ParamType type, pw_options const& opt, std::string const& label)
	{
		param_wgt* pw = nullptr;
		switch(type)
		{
			case ParamType::Boolean:
			pw = new boolean_par_wgt();
			break;

			case ParamType::Integer:
			pw = new integer_par_wgt();
			break;

			case ParamType::RealNumber:
			pw = new realnum_par_wgt();
			break;

			case ParamType::String:
			pw = new string_par_wgt();
			break;

			case ParamType::Enumeration:
			pw = new enum_par_wgt();
			break;

			case ParamType::Vector2:
			pw = new vector_par_wgt();
			break;

			case ParamType::List:
			pw = new container_par_wgt();
			break;
		}

		if(pw == nullptr)
		{
			return nullptr;
		}

		pw->initialize(opt, label);
		return pw;
	}

	void param_wgt::connect_changed_handler(boost::function< void() > const& handler)
	{
		Wt::WFormWidget* fw = dynamic_cast<Wt::WFormWidget*>(m_base_impl);
		if(fw)
		{
			fw->changed().connect(std::bind(handler));
		}
	}
}




