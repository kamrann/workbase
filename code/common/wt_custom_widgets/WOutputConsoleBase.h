// WOutputConsoleBase.h

#ifndef __WB_WTCUSTOM_OUTPUT_CONSOLE_BASE_H
#define __WB_WTCUSTOM_OUTPUT_CONSOLE_BASE_H

#include <string>
#include <vector>


class WOutputConsoleBase
{
public:
	virtual void add_element(std::string elem);
	virtual void clear();

protected:
	virtual void update_display() = 0;

protected:
	std::vector< std::string > m_elements;
};


#endif


