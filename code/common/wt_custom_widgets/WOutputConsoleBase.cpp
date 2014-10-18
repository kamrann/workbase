// WOutputConsoleBase.cpp

#include "WOutputConsoleBase.h"


void WOutputConsoleBase::add_element(std::string elem)
{
	m_elements.push_back(std::move(elem));
	update_display();
}

void WOutputConsoleBase::clear()
{
	m_elements.clear();
	update_display();
}


