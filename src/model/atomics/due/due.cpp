#include <random>
#include <string>
#include <stdlib.h>
#include <time.h>


#include "message.h" // class ExternalMessage, InternalMessage
#include "parsimu.h" // ParallelMainSimulator::Instance().getParameter( ... )
#include "real.h"
#include "tuple_value.h"
#include "strutil.h"

#include "due.h"

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



due::due(const std::string &name): Atomic(name),
out(addOutputPort( "out" )),
timeAdvanceGenerator(0.0,1.0)
{
    std::mt19937::result_type seed = time(nullptr);
    rnd.seed(seed);
    lambda = std::stod(ParallelMainSimulator::Instance().getParameter( description(), "lambda" ));

}

Model &due::initFunction(){

    double exp = log(this->timeAdvanceGenerator(this->rnd)) / this->lambda;
    double ms, sc; 
    ms = std::modf(exp, &sc);

    std::string toTime = "0:0:"+ std::to_string(sc)+":"+ std::to_string(ms);

    this->elapsed = VTime::Zero;
    this->sigma = toTime;
    this->timeLeft = this->sigma - this->elapsed;

    this->message_id = 0;


    holdIn( AtomicState::active, this->sigma  );

    return *this;
}

Model &due::externalFunction( const ExternalMessage &msg ){
    
    return *this;
}

Model &due::internalFunction(const InternalMessage &msg ){


    double exp = log(this->timeAdvanceGenerator(this->rnd)) / this->lambda;
    double ms, sc; 
    ms = std::modf(exp, &sc);

    std::string toTime = "0:0:"+ std::to_string(sc)+":"+ std::to_string(ms);

    this->sigma = toTime;
    this->message_id ++;

    cout << msg.time() << ".internal called. Sigma: "<< this->sigma.asString() << endl;

    holdIn( AtomicState::active, this->sigma  );

    return *this;
}

Model &due::outputFunction( const CollectMessage &msg ){

    int val = this->message_id;

    sendOutput( msg.time(), out, val );

    cout << msg.time() << ". out called. MessageId:" << val <<  endl;

    return *this;
}

due::~due(){}
