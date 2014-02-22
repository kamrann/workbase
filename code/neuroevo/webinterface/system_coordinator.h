// system_coordinator.h

#ifndef __SYSTEM_COORDINATOR_H
#define __SYSTEM_COORDINATOR_H

#include <functional>


namespace Wt {
	class WWidget;
}

class system_coordinator
{
public:
	virtual std::pair< Wt::WWidget*, Wt::WWidget* > initialize() = 0;
	virtual void restart() = 0;
};


class agent_controller_base
{
public:
	virtual bool is_synchronous() const = 0;
};


#endif

