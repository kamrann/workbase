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
	virtual void cancel() = 0;
	virtual void restart() = 0;

	virtual void register_system_finish_callback(std::function< void() > cb) = 0;

	virtual ~system_coordinator() = default;
};


class agent_controller_base
{
public:
	virtual bool is_synchronous() const = 0;
};


#endif

