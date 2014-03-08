#ifndef my_udpsink_h
#define my_udpsink_h

#include <stdio.h>
#include "agent.h"

class my_UdpSink : public Agent
{
public:
 		my_UdpSink() : Agent(PT_UDP), id(0), expected_(0) {} 		
        	void recv(Packet*, Handler*);
        	int command(int argc, const char*const* argv);
protected:
		char tbuf[100];
		FILE *tFile;
		int  id;
		int seqno_;
		int expected_;
};

#endif
