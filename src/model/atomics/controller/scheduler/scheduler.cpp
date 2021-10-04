/** include files **/
#include <random>
#include <string>
#include <stdlib.h>
#include <time.h>

#include "message.h" // class ExternalMessage, InternalMessage
#include "parsimu.h" // ParallelMainSimulator::Instance().getParameter( ... )
#include "real.h"
#include "tuple_value.h"
#include "distri.h"        // class Distribution
#include "strutil.h"

#include "scheduler.h" // Cambiar nombre, base header


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
scheduler::scheduler( const std::string &name ) : 
	Atomic( name ),
	trow(addOutputPort( "trow" )),
    queuePort(addInputPort("queuePort")),
	protocolPort(addInputPort("protocolPort")),
	ack(addOutputPort("ack")),
{
	for(int i=0; i<4; i++){

		relayOut.push_back(addOutputPort( "relay"+std::to_string(i+1)+"Out" ));
		relayIn.push_back(addOutputPort( "relay"+std::to_string(i+1)+"In" ));
	}
	maxRetransmission = str2Int( ParallelMainSimulator::Instance().getParameter( description(), "N" ) );
	std::mt19937::result_type seed = time(nullptr);
    rnd.seed(seed);
    
	
}

/*******************************************************************
* Function Name: initFunction
********************************************************************/
Model &scheduler::initFunction()
{
	// [(!) Initialize common variables]
	this->elapsed  = VTime::Zero;
 	this->timeLeft = VTime::Inf;
 	// this->sigma = VTime::Inf; // stays in active state until an external event occurs;
 	this->sigma    = VTime::Inf; // force an internal transition in t=0;

 	// TODO: add init code here. (setting first state, etc
	this->frame = -1;
	this->message_identifier = 0;

 	// set next transition
 	holdIn( AtomicState::passive, this->sigma  ) ;
	return *this ;
}

/*******************************************************************
* Function Name: externalFunction
* Description: This method executes when an external event is received.
********************************************************************/
Model &scheduler::externalFunction( const ExternalMessage &msg )
{
#if VERBOSE
	PRINT_TIMES("dext");
#endif
	//[(!) update common variables]	
	this->sigma    = nextChange();	
	this->elapsed  = msg.time()-lastChange();	
 	this->timeLeft = this->sigma - this->elapsed;

	if(msg.port() == queuePort){

		
	
		this->message_identifier = msg.senderModelId();
				
		this->number_of_retransmission = packet[2];		
		
		holdIn( AtomicState::active, VTime::Zero );
			
	}else if(msg.port() == protocolPort){

		Real val = msg.value();
		int identifier = msg.senderModelId();
		
		Real cycle = Real::from_value(val.value());

		if(this->priority.size() == 4){
			for(int i = 0; i < this->priority.size(); i++){
				this->priority.pop();
			}
		}

		if(this->cycle == Real(1)){
			this->priority.push_back(identifier);
		}

	}else{

		Tuple<Real> packet = Tuple<Real>::from_value(msg.value());
		this->message_identifier = msg.senderModelId();

		Real outcome = packet[1];
		this->relay_pdr[this->message_identifier] += static_cast<float>(packet[0]) / this->delivered[msg.senderModelId()]; // testear que msg.senderModelId() devuelva id del relay
		this->number_of_retransmission = packet[2];	


		if(outcome == Real(0)){
			this->delivered[this->message_identifier]++;
		}else{
			holdIn( AtomicState::active, VTime::Zero );

		}

		

	}

	

	
	return *this ;
}

/*******************************************************************
* Function Name: internalFunction
* Description: This method executes when the TA has expired, right after the outputFunction has finished.
* The new state and TA should be set.
********************************************************************/
Model &scheduler::internalFunction( const InternalMessage &msg )
{
#if VERBOSE
	PRINT_TIMES("dint");
#endif
	//TODO: implement the internal function here
	this->update = false;
	this->sigma = VTime::Inf; // stays in passive state until an external event occurs;
	holdIn( AtomicState::passive, this->sigma );
	return *this;

}

/*******************************************************************
* Function Name: outputFunction
* Description: This method executes when the TA has expired. After this method the internalFunction is called.
* Output values can be send through output ports
********************************************************************/
Model &scheduler::outputFunction( const CollectMessage &msg )
{
	//TODO: implement the output function here
	// remember you can use sendOutput(time, outputPort, value) function.
	// sendOutput( msg.time(), out, 1) ;
	// value could be a tuple with different number of elements: 
	// Tuple<Real> out_value{Real(value), 0, 1};
	
	if(Real(this->maxRetransmission) >= this->number_of_retransmission){
		int outPort = -1;
		for(int i =0; i < this->relayOut.size(); i++){
			InfluenceList relays = relayOut[i].influences()
			InfluenceList::iterator cursor;
			for( cursor = relays.begin() ;cursor != relays.end() && (*cursor)->modelId() != this->message_identifier; cursor++ ) ;
			outPort = (*cursor)->modelId() == this->message_identifier?i:outPort;

		}

		sendOutput(msg.time(),relayOut[outPort] , this->number_of_retransmission);
			
	}else{

		sendOutput(msg.time(),trow, this->number_of_retransmission);
	}

	
	
	
	
	
	return *this;

}

scheduler::~scheduler()
{
	//TODO: add destruction code here. Free distribution memory, etc. 
}
