// rtp_agent.h

#ifndef __NE_RTP_AGENT_H
#define __NE_RTP_AGENT_H

#include <vector>
#include <functional>
#include <memory>


namespace rtp {

	typedef int agent_sensor_id;
	typedef std::vector< agent_sensor_id > agent_sensor_list;
	typedef std::vector< std::string > agent_sensor_name_list;	// Ordered - index corresponds to agent_sensor_id


	class i_agent
	{
	public:
		virtual std::string get_name() const = 0;
		virtual agent_sensor_list get_mapped_inputs(std::vector< std::string > const& named_inputs) const = 0;
		virtual double get_sensor_value(agent_sensor_id const& sensor) const = 0;

	public:
		virtual ~i_agent() {}
	};

	class i_agent_factory
	{
	public:
		virtual std::unique_ptr< i_agent > create() const = 0;

	public:
		virtual ~i_agent_factory() {}
	};

	class agent_factory: public i_agent_factory
	{
	public:
		typedef std::function< std::unique_ptr< i_agent >() > create_fn_t;

		agent_factory(create_fn_t fn): m_fn(fn)
		{}

		virtual std::unique_ptr< i_agent > create() const
		{
			return m_fn();
		}

	private:
		create_fn_t m_fn;
	};

}


#endif

