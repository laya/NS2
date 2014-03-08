#include <stdio.h>
#include <stdlib.h>
#include "ccn_sink.h"
#include "ip.h"
#include "udp.h"
#include "rtp.h"

static class ccn_sinkClass : public TclClass {
public:
        ccn_sinkClass() : TclClass("Agent/ccn_sink") {}
        TclObject* create(int, const char*const*) {
                return (new ccn_sink);
        }
} class_ccn_sink;

void ccn_sink::recv(Packet* pkt, Handler*)
{
        hdr_ip* iph=hdr_ip::access(pkt);
  	hdr_cmn* hdr=hdr_cmn::access(pkt);
	hdr_rtp* rtp = hdr_rtp::access(pkt);
	seqno_ = rtp->seqno();
	
	if (expected_ >= 0) {
		int loss = seqno_ - expected_;
		if (loss > 0) {
			while (loss>0){
				fprintf(tFile,"%-16f id %-16d lost %-16d\n", Scheduler::instance().clock(), id++, 9999);	
				loss-=1;
				if(loss==0)
					fprintf(tFile,"%-16f id %-16d udp %-16d\n", Scheduler::instance().clock(), id++, hdr->size());
			}
		} else {
			fprintf(tFile,"%-16f id %-16d udp %-16d\n", Scheduler::instance().clock(), id++, hdr->size());
		}
	}
	expected_ = seqno_ + 1;
	
  	if (app_)
               app_->recv(hdr_cmn::access(pkt)->size());

        Packet::free(pkt);
}

int ccn_sink::command(int argc, const char*const* argv)
{
	Tcl& tcl = Tcl::instance();
	
	if (strcmp(argv[1], "set_trace_filename") == 0) {
		strcpy(tbuf, argv[2]);
		tFile = fopen(tbuf, "w");
		return (TCL_OK);
	}  
	
	if (strcmp(argv[1], "closefile") == 0) {
		
		fclose(tFile);
		return (TCL_OK);
	}
	
	return (Agent::command(argc, argv));
}
