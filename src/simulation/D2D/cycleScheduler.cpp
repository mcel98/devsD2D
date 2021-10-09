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
    ProtocolOut(addOutputPort( "ProtocolOut" )),
	ProtocolIn(addOutputPort( "ProtocolIn" )),
	dist(0.0,1.0),
    frequency_time(0,0,1,0)
{
    
    
	cycle_rate = std::stof( ParallelMainSimulator::Instance().getParameter( description(), "c" ) );


}

/*******************************************************************
* Function Name: initFunction
********************************************************************/
Model &cycleScheduler::initFunction()
{

	this->send_info = false;

	std::random_device rd;
    this->rnd.seed(rd());
 	this->sigma    = this->frequency_time; // force an internal transition in t=0;
 	// set next transition

	std::vector<Real> duty_window(4,Real(0)); 
	this->send_info = false;	
    
	for(int i =0; i<4;i++){
		float round = this->dist(rnd);
		if(round <= this->cycle_rate){
           	duty_window[i] = Real(1);
		}
	}
	this->duty = duty_window;
	

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
	PRINT_TIMES("signaling scheduler info:");
#endif
	this->send_info = true;	
	
	return *this ;
}

/*******************************************************************
* Function Name: internalFunction
* Description: This method executes when the TA has expired, right after the outputFunction has finished.
* The new state and TA should be set.
********************************************************************/
Model &cycleScheduler::internalFunction( const InternalMessage &msg )
{

	//TODO: implement the internal function here
		 
	this->send_info = false;    
	std::vector<Real> duty_window(4,Real(0)); 	
    
	for(int i =0; i<4;i++){
		float round = this->dist(rnd);
		if(round <= this->cycle_rate){
           	duty_window[i] = Real(1);
		}
	}
	this->duty = duty_window;
	

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
	if(this->send_info){
		std::cout << "window sent" << std::endl;
		auto linked = ProtocolOut.influences();
		auto relay_id= linked.front()->modelId();
		this->duty.push_back(Real(0));
		this->duty.push_back(Real(relay_id));
		this->duty.push_back(Real(0.0));
		Tuple<Real> out_value(&this->duty);
    	sendOutput( msg.time(), ProtocolOut, out_value);
	}
	return *this;

}


cycleScheduler::~cycleScheduler()
{
	//TODO: add destruction code here. Free distribution memory, etc. 
}
