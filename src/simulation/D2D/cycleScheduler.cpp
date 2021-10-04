/** include files **/
#include <random>
#include <string>
#include <stdlib.h>
#include <time.h>
#include <bitset>

#include "message.h" // class ExternalMessage, InternalMessage
#include "parsimu.h" // ParallelMainSimulator::Instance().getParameter( ... )
#include "real.h"
#include "tuple_value.h"
#include "strutil.h"

#include "cycleScheduler.h" // Cambiar nombre, base header


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

/** public functions **/

/*******************************************************************
* Function Name: [#MODEL_NAME#]
* Description: constructor
********************************************************************/
cycleScheduler::cycleScheduler( const std::string &name ) : 
	Atomic( name ),
    updatePort(addOutputPort( "updatePort" )),
	dist(0.0,1.0),
    frequency_time(0,0,0,25)
{
    std::mt19937::result_type seed = time(nullptr);
    duty = Real(0); 

    rnd.seed(seed);
    
	cycle_rate = std::stof( ParallelMainSimulator::Instance().getParameter( description(), "c" ) );


}

/*******************************************************************
* Function Name: initFunction
********************************************************************/
Model &cycleScheduler::initFunction()
{

 	this->sigma    = this->frequency_time; // force an internal transition in t=0;

    float round = this->dist(rnd);
    this->duty = Real(0);
    if(duty < this->cycle_rate){
        this->duty = Real(1);
    }

 	// set next transition
 	holdIn( AtomicState::active, this->sigma  ) ;
	return *this ;
}

/*******************************************************************
* Function Name: externalFunction
* Description: This method executes when an external event is received.
********************************************************************/
Model &cycleScheduler::externalFunction( const ExternalMessage &msg )
{
#if VERBOSE
	PRINT_TIMES("dext");
#endif
	//[(!) update common variables]	
	return *this ;
}

/*******************************************************************
* Function Name: internalFunction
* Description: This method executes when the TA has expired, right after the outputFunction has finished.
* The new state and TA should be set.
********************************************************************/
Model &cycleScheduler::internalFunction( const InternalMessage &msg )
{
#if VERBOSE
	PRINT_TIMES("cycleScheduler");
#endif
	//TODO: implement the internal function here

        float round = this->dist(rnd);
        this->duty = Real(0);
        if(round <= this->cycle_rate){
            this->duty = Real(1);
		}

	holdIn( AtomicState::active, this->frequency_time );
	return *this;

}

/*******************************************************************
* Function Name: outputFunction
* Description: This method executes when the TA has expired. After this method the internalFunction is called.
* Output values can be send through output ports
********************************************************************/
Model &cycleScheduler::outputFunction( const CollectMessage &msg )
{
	std::cout << "updating scheduler" << std::endl;
    sendOutput( msg.time(), updatePort, this->duty);
	
	return *this;

}


cycleScheduler::~cycleScheduler()
{
	//TODO: add destruction code here. Free distribution memory, etc. 
}
