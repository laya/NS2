#include <sys/types.h>
#include <sys/stat.h> 
#include <stdio.h>
#include "config.h"
#ifdef HAVE_NETINET_IN_H
#include <netinet/in.h>
#endif
#include "random.h"
#include "object.h"
#include "trafgen.h"

struct tracerec {
        float trec_time;
	u_int32_t trec_size; 
};

class my_TraceFile : public NsObject {
 public:
	my_TraceFile();
	void get_next(int&, struct tracerec&); 
	int setup();  
	int command(int argc, const char*const* argv);
	int get_a();			
 private:
	void recv(Packet*, Handler*); 
        int status_; 
	char *name_;  
	int nrec_;    
	struct tracerec *trace_; 
	int a_;
};



class ccn_trafficTrace : public TrafficGenerator {
 public:
	ccn_trafficTrace();
	int command(int argc, const char*const* argv);
	virtual double next_interval(int &);
 protected:
	void timeout();
	my_TraceFile *tfile_;
	struct tracerec trec_;
	int ndx_;
	void init();
};


static class my_TraceFileClass : public TclClass {
 public:
	my_TraceFileClass() : TclClass("my_Tracefile") {}
	TclObject* create(int, const char*const*) {
		return (new my_TraceFile());
	}
} class_my_tracefile;

my_TraceFile::my_TraceFile() : status_(0)
{
	a_=0;
}

int my_TraceFile::get_a()
{
	return a_;
}

int my_TraceFile::command(int argc, const char*const* argv)
{

	if (argc == 3) {
		if (strcmp(argv[1], "filename") == 0) {
			name_ = new char[strlen(argv[2])+1];
			strcpy(name_, argv[2]);
			return(TCL_OK);
		}
	}
	return (NsObject::command(argc, argv));
}

void my_TraceFile::get_next(int& ndx, struct tracerec& t)
{
	t.trec_time = trace_[ndx].trec_time;
	t.trec_size = trace_[ndx].trec_size;
   
        
	if (ndx++ == nrec_){
		ndx = 0;
		a_= 1;
	}
}

int my_TraceFile::setup()
{
	tracerec* t;
	struct stat buf;
	int i;
	unsigned long size;
	double time;
	FILE *fp;

	if((fp = fopen(name_, "r")) == NULL) {
		printf("can't open file %s\n", name_);
		return -1;
	}
	
	nrec_ = 0;
	
	while (!feof(fp)){
		fscanf(fp, "%lf %ld", &time, &size);
		
		nrec_++;
	}
	
	nrec_=nrec_-2;	
	printf("%d records\n", nrec_);

	rewind(fp);
	trace_ = new struct tracerec[nrec_];

	for (i = 0, t = trace_; i < nrec_; i++, t++){
		fscanf(fp, "%lf %ld", &time, &size);
		t->trec_time = time;
		t->trec_size = size;;
	}

	return 0;
}

void my_TraceFile::recv(Packet*, Handler*)
{

        abort();
}



static class ccn_trafficTraceClass : public TclClass {
 public:
	ccn_trafficTraceClass() : TclClass("Application/Traffic/my_Trace") {}
	TclObject* create(int, const char*const*) {
	        return(new ccn_trafficTrace());
	}
} class_ccn_traffictrace;

ccn_trafficTrace::ccn_trafficTrace()
{
	tfile_ = (my_TraceFile *)NULL;
}

void ccn_trafficTrace::init()
{
	if (tfile_) 
		ndx_ = tfile_->setup();
}

int ccn_trafficTrace::command(int argc, const char*const* argv)
{
	Tcl& tcl = Tcl::instance();
	
	if (argc == 3) {
		if (strcmp(argv[1], "attach-tracefile") == 0) {
			tfile_ = (my_TraceFile *)TclObject::lookup(argv[2]);
			if (tfile_ == 0) {
				tcl.resultf("no such node %s", argv[2]);
				return(TCL_ERROR);
			}
			return(TCL_OK);
		}
	}

	return (TrafficGenerator::command(argc, argv));

}

void ccn_trafficTrace::timeout()
{      
        if (! running_)
                return;
        
        if (tfile_->get_a()==1){
        	running_=0;
        	return;
	}
	
	agent_->set_pkttype(PT_VIDEO);	
 	agent_->sendmsg(28+size_); 
	

        nextPkttime_ = next_interval(size_);
        

        timer_.resched(nextPkttime_);
}


double ccn_trafficTrace::next_interval(int& size)
{
        tfile_->get_next(ndx_, trec_);
	size = trec_.trec_size;
	return(((double)trec_.trec_time)/1000000.0); 
}
