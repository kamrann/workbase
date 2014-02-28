// sync_issue_par.cpp

#include "sync_issue_par.h"
#include "fixed_or_random_par.h"

#include <Wt/WSpinBox>


sync_issue_widget::sync_issue_widget()
{
	// Add a combo box as the first child widget
/*	Wt::WComboBox* sel_box = new Wt::WComboBox();
	sel_box->addItem("Nothing");
	sel_box->addItem("Something");
	addWidget(sel_box);
	*/
	Wt::WPushButton* btn = new Wt::WPushButton("Click Me");
	addWidget(btn);

	// Initially add a placeholder as the second
	Wt::WContainerWidget* placeholder = new Wt::WContainerWidget();
	//
	//placeholder->setLayout(new Wt::WGridLayout());
	//
	addWidget(placeholder);

	// Now connect a handler to the combo box change event
/*	sel_box->changed().connect(std::bind([=](){

		// Get the existing widget in the second position of the container
		Wt::WWidget* old = widget(1);
		// Remove and delete it
		removeWidget(old);
		delete old;

		// Depending on combo box selection...
		bool something = (sel_box->currentIndex() == 1);
		Wt::WWidget* new_child;
		if(something)
		{
			// Create either a WText
			new_child = new Wt::WText("Hi");
		}
		else
		{
			// Or a nested empty container
			Wt::WContainerWidget* cont = new Wt::WContainerWidget(); // NO ISSUE, unless...
			// With a grid layout (this matters!!)
			cont->setLayout(new Wt::WGridLayout());	// Now, ISSUE!
			new_child = cont;
		}
		// Add insert into second position
		insertWidget(1, new_child);
	}));

	// Trigger the handler to reflect the initial combo box selection
	sel_box->changed().emit();
*/

	btn->clicked().connect([=](Wt::WMouseEvent e){

		// Get the existing widget in the second position of the container
		Wt::WWidget* old = widget(1);
		// Remove and delete it
		removeWidget(old);
		delete old;

		Wt::WContainerWidget* cont = new Wt::WContainerWidget(); // NO ISSUE, unless...
		// With a grid layout (this matters!!)
		cont->setLayout(new Wt::WGridLayout());	// Now, ISSUE!
		insertWidget(1, cont);
	});

	// Trigger the handler to reflect the initial combo box selection
	btn->clicked().emit(Wt::WMouseEvent());
}

i_param_widget* sync_issue_param_type_2::create_widget(rtp_param_manager* mgr) const
{
	//
	rtp_param_widget< Wt::WContainerWidget >* w = new rtp_param_widget< Wt::WContainerWidget >(this);
	//

	rtp_named_param sel_prm = provide_selection_param();
	i_param_widget* sel_w = sel_prm.type->create_widget(mgr);
	mgr->register_widget(sel_prm.name, sel_w);

	//
	w->addWidget(sel_w->get_wt_widget());
	//

	Wt::WComboBox* sel_box = dynamic_cast< Wt::WComboBox* >(w->widget(0));

	w->addWidget(new Wt::WContainerWidget());

	sel_box->changed().connect(std::bind([=](){
		//
		Wt::WWidget* old = w->widget(1);
		w->removeWidget(old);
		delete old;

		//w->insertWidget(1, provide_nested_param(mgr)->create_widget(mgr)->get_wt_widget());
		bool something = (sel_box->currentIndex() == 1);
		Wt::WWidget* new_child;
		//rtp_param_type* sub;
		if(something)
		{
			/*
			rtp_param_type* sub = new rtp_integer_param_type();
			i_param_widget* pw = sub->create_widget(mgr);
			new_child = pw->get_wt_widget();
			*/
			new_child = new Wt::WText("Hi");
		}
		else
		{
			//new_child = new Wt::WContainerWidget(); - NO ISSUE
			new_child = (new rtp_staticparamlist_param_type(rtp_named_param_list()))->create_widget(mgr)->get_wt_widget();
		}
		w->insertWidget(1, new_child);//sub->create_widget(mgr)->get_wt_widget());
		//w->insertWidget(1, something ? (Wt::WWidget*)(new Wt::WSpinBox()) : (Wt::WWidget*)(new Wt::WContainerWidget()));
		//
	}));

	sel_box->changed().emit();

	return w;
}
/*
rtp_param_type* sync_issue_param_type_2::provide_nested_param(rtp_param_manager* mgr) const
{
	bool something = boost::any_cast< bool >(mgr->retrieve_param("Sync Issue 2"));
	if(something)
	{
		return new rtp_integer_param_type();
	}
	else
	{
		return new rtp_staticparamlist_param_type(rtp_named_param_list());
	}
}
*/



