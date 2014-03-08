#ifndef ccn_sink_h
#define ccn_sink_h

#include <stdio.h>
#include "agent.h"

class ccn_sink : public Agent
{
public:
 		ccn_sink() : Agent(PT_UDP), id(0), expected_(0) {} 		
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
