// properties_widget.h

#ifndef __WB_PROPERTIES_WIDGET_H
#define __WB_PROPERTIES_WIDGET_H

#include "../rtp_interface/rtp_properties.h"

#include <memory>


class i_properties_widget
{
public:
	virtual void clear_content() = 0;
	virtual void reset(std::shared_ptr< rtp::i_properties const > props = nullptr) = 0;
	virtual void register_data(std::shared_ptr< rtp::i_property_values const > vals) = 0;
};


#endif


