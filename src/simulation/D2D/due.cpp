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
    std::mt19937::result_type seed = time(NULL);
    rnd.seed(seed);
    lambda = std::stod(ParallelMainSimulator::Instance().getParameter( description(), "lambda" ));

}

Model &due::initFunction(){


    double exp = -1 * log(this->timeAdvanceGenerator(this->rnd)) / this->lambda;
    std::cout << exp << endl;
    double ms, sc;
    ms = std::modf(exp, &sc);
    std::cout << ms << "|" << sc << endl; 

    int seconds = (int)sc;
    int milliseconds = (int)(ms *  1000);
    std::cout << seconds << milliseconds <<endl;
    this->elapsed = VTime::Zero;
    this->sigma = VTime(0,0,seconds, milliseconds);
    this->timeLeft = this->sigma - this->elapsed;

    this->message_id = 0;



    holdIn( AtomicState::active, this->sigma  );

    return *this;
}

Model &due::externalFunction( const ExternalMessage &msg ){
    
    return *this;
}

Model &due::internalFunction(const InternalMessage &msg ){

    #if VERBOSE
	    PRINT_TIMES("int");
    #endif

    double exp = -1 * log(this->timeAdvanceGenerator(this->rnd)) / this->lambda;
    std::cout << exp << endl;
    double ms, sc; 
    ms = std::modf(exp, &sc);

    int seconds = (int)sc;
    int milliseconds = (int)(ms * 1000);


    this->sigma = VTime(0,0,seconds, milliseconds);;
    this->message_id ++;


    cout << msg.time() << ".internal called. Sigma: "<< this->sigma.asString() << endl;

    holdIn( AtomicState::active, this->sigma  );

    return *this;
}

Model &due::outputFunction( const CollectMessage &msg ){

    Tuple<Real> out_value{Real(this->message_id), 0};

    sendOutput( msg.time(), out, out_value );

    cout << msg.time() << ". out called. MessageId:" << this->message_id <<  endl;

    return *this;
}

due::~due(){}
