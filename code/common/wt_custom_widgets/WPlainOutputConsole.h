// WPlainOutputConsole.h

#ifndef __WB_WTCUSTOM_PLAIN_OUTPUT_CONSOLE_H
#define __WB_WTCUSTOM_PLAIN_OUTPUT_CONSOLE_H

#include "WOutputConsoleBase.h"

#include <Wt/WTableView>
#include <Wt/WStandardItemModel>


class WPlainOutputConsole:
	public Wt::WTableView,
	public WOutputConsoleBase
{
public:
	WPlainOutputConsole();

public:
	void add_element(std::string elem) override;
	void clear() override;

protected:
	virtual void update_display() override;
	virtual void layoutSizeChanged(int width, int height) override;

protected:
	std::string get_full_text() const;

private:
//	Wt::WStringListModel* m_model;
	Wt::WStandardItemModel* m_model;
};


#endif


