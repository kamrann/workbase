
#include <Wt/WApplication>
#include <Wt/WPushButton>
#include <Wt/WContainerWidget>
#include <Wt/WTabWidget>
#include <Wt/WSpinBox>
#include <Wt/WTextArea>
#include <Wt/WGridLayout>

#include <string>
#include <sstream>
#include <iostream>

using namespace Wt;
using namespace std;


class SyncIssueApp: public WApplication
{
public:
	SyncIssueApp(const WEnvironment& env): WApplication(env)
	{
		WTabWidget* tab_widget = new WTabWidget(root());

		// NOTE: If this tab is added last, the issue goes away.
		tab_widget->addTab(new WContainerWidget, "Empty Tab");

		WContainerWidget* issue_tab = new WContainerWidget();

		// Just for feedback purposes
		WTextArea* output = new WTextArea();
		output->setInline(false);
		output->setReadOnly(true);
		output->resize(400, 300);
		output->setText("First, check that the spin box is working as expected. Then click the button.\n");

		// Spin box which will lose sync
		WSpinBox* edit = new WSpinBox();
		issue_tab->addWidget(edit);

		// Handler for spin box change event
		edit->changed().connect(std::bind([=](){

			// Get the spin box value and output it to the text area
			std::string text = edit->text().toUTF8();
			std::stringstream output_text;
			output_text << output->text().toUTF8() << "Spin changed (" << text << ")." << std::endl;
			output->setText(output_text.str());
		}));

		// Button to trigger the issue
		WPushButton* btn = new WPushButton("Click Me");
		issue_tab->addWidget(btn);

		// Add an empty container with a layout, which will be removed and deleted when the button is clicked
		WContainerWidget* cont = new WContainerWidget();
		// Without adding this layout, there is no issue!
		cont->setLayout(new WGridLayout());
		issue_tab->addWidget(cont);

		// Handler for the button. This is what causes the problem (but is only seen after switching tabs).
		btn->clicked().connect([=](WMouseEvent e){

			std::stringstream output_text;
			output_text << output->text().toUTF8() << "Button clicked, replacing empty container." << std::endl;
			output_text << "Switch to the empty tab and back, then modify the spin box." << std::endl;
			output->setText(output_text.str());

			// Get the empty container with layout that we added above
			WWidget* cont = issue_tab->widget(2);
			// Remove and delete it
			issue_tab->removeWidget(cont);
			delete cont;

			btn->disable();
		});

		// Add the output text area at the bottom
		issue_tab->addWidget(output);

		// NOTE: Without PreLoading, there is no issue.
		tab_widget->addTab(issue_tab, "Issue Tab", WTabWidget::PreLoading);
	}
};


WApplication* createApplication(const WEnvironment& env)
{
	return new SyncIssueApp(env);
}

int main(int argc, char* argv[])
{
	return WRun(argc, argv, &createApplication);
}

