#ifndef ns_ccn_h
#define ns_ccn_h

#include "udp.h"

class ccnAgent : public UdpAgent {
public:
	ccnAgent();
	virtual void sendmsg(int nbytes, AppData* data, const char *flags = 0);
	virtual int command(int argc, const char*const* argv);
protected:
	
	int id_;	
	char BWfile[100];
	FILE *BWFile;
	int openfile;
	
	nsaddr_t index_;                  
};

#endif
