// WOutputConsoleBase.cpp

#include "WOutputConsoleBase.h"


void WOutputConsoleBase::add_element(std::string elem)
{
	// TODO: temp hack for clearing console 
	if(elem.empty())
	{
		m_elements.clear();
	}
	else
	{
		m_elements.push_back(std::move(elem));
	}
	update_display();
}

void WOutputConsoleBase::clear()
{
	m_elements.clear();
	update_display();
}


