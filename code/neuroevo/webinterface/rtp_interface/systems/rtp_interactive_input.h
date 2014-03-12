// rtp_interactive_input.h

#ifndef __NE_RTP_INTERACTIVE_INPUT_H
#define __NE_RTP_INTERACTIVE_INPUT_H


// TODO: This is temp implementation
class interactive_input
{
public:
	interactive_input(size_t k = 0, bool d = false): key_index(k), down(d)
	{}

public:
	size_t key_index;
	bool down;
};


#endif

