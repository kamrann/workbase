// par_wgt_holder.h

#ifndef __WB_PARAM_WIDGET_HOLDER_H
#define __WB_PARAM_WIDGET_HOLDER_H

#include <Wt/WContainerWidget>


namespace Wt {
	class WText;
	class WPushButton;
	class WPopupMenu;
}

namespace prm
{
	class par_wgt_holder:
		public Wt::WContainerWidget
	{
	public:
		par_wgt_holder();

	public:
		virtual void set_label(std::string text);
		virtual void set_status_text(std::string text);
		virtual void set_menu(Wt::WPopupMenu* menu);
		virtual void enable_border(bool enable) = 0;
		virtual void set_contents(Wt::WWidget* wgt) = 0;

	protected:
		bool m_stuff_enabled;	 // TODO: temp
		Wt::WText* m_label;
		Wt::WText* m_status;
		Wt::WPushButton* m_menu_btn;
	};

	class boxed_holder:
		public par_wgt_holder
	{
	public:
		boxed_holder();

	public:
		virtual void enable_border(bool enable);
		virtual void set_contents(Wt::WWidget* wgt);
	};

	class inline_holder:
		public par_wgt_holder
	{
	public:
		inline_holder();

	public:
		virtual void enable_border(bool enable);
		virtual void set_contents(Wt::WWidget* wgt);
	};
}


#endif


