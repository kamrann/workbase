// rtp_agent.h

#ifndef __NE_RTP_AGENT_H
#define __NE_RTP_AGENT_H


typedef std::vector< std::string > agent_sensor_list;


class i_agent
{
public:
	virtual ~i_agent() {}
};

class i_agent_factory
{
public:
	virtual ~i_agent_factory() {}

	virtual i_agent* create() = 0;
};


#endif

