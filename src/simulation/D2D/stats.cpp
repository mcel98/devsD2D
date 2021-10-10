#include <random>
#include <string>
#include <stdlib.h>
#include <time.h>


#include "message.h" // class ExternalMessage, InternalMessage
#include "parsimu.h" // ParallelMainSimulator::Instance().getParameter( ... )
#include "real.h"
#include "tuple_value.h"
#include "strutil.h"

#include "stats.h"

#define VERBOSE true

#define PRINT_TIMES(f) {\
	VTime timeleft = nextChange();\
	VTime elapsed  = msg.time() - lastChange();\
	VTime sigma    = elapsed + timeleft;\
	cout << f << "@" << msg.time() <<\
		" - timeleft: " << timeleft <<\
		" - elapsed: " << elapsed <<\
		" - sigma: " << sigma << endl;\
}



stats::stats(const std::string &name): Atomic(name),
out(addOutputPort("out")),
success(addInputPort( "success" )),
fail(addInputPort( "fail" ))
{


}

Model &stats::initFunction(){

    this->sigma = VTime::Inf;
    this->type = false;

    holdIn( AtomicState::active, this->sigma  );

    return *this;
}

Model &stats::externalFunction( const ExternalMessage &msg ){


    if(msg.port() == fail){

        this->failed = msg.time().asSecs();
        this->type = false;

    }else{

        
        Tuple<Real> message{Real(msg.time().asSecs()), Tuple<Real>::from_value(msg.value())[1].value()};
        this->delivered = message;
        this->type = true;

    }
    this->sigma = VTime::Zero;
    holdIn( AtomicState::active, this->sigma  );
    return *this;
}

Model &stats::internalFunction(const InternalMessage &msg ){

    #if VERBOSE
	    PRINT_TIMES("int");
    #endif

    this->sigma = VTime::Inf;
    holdIn( AtomicState::active, this->sigma  );

    return *this;
}

Model &stats::outputFunction( const CollectMessage &msg ){
    if(type){
        sendOutput( msg.time(), out, this->delivered );

    }else{

        sendOutput( msg.time(), out, Real(failed) );
    }

    cout << msg.time() << "outcome recorded" <<  endl;

    return *this;
}

stats::~stats(){}
