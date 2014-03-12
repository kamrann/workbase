// rtp_interactive_agent.h

#ifndef __NE_RTP_INTERACTIVE_AGENT_H
#define __NE_RTP_INTERACTIVE_AGENT_H


class interactive_input;

// TODO: This is a temp solution
class i_interactive_agent
{
public:
	virtual void register_input(interactive_input const& input) = 0;
};



#endif

