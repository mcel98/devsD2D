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
	relayIn(addInputPort("relayIn")),
	relayOut1(addOutputPort("relay1Out")),
	relayOut2(addOutputPort("relay2Out")),
	relayOut3(addOutputPort("relay3Out")),
	relayOut4(addOutputPort("relay4Out"))
{
	maxRetransmission = str2Int( ParallelMainSimulator::Instance().getParameter( description(), "N" ) );

    
	
}

/*******************************************************************
* Function Name: initFunction
********************************************************************/
Model &scheduler::initFunction()
{
 	this->sigma    = VTime::Zero; // force an internal transition in t=0;

 	// TODO: add init code here. (setting first state, etc
	
	auto linked = relayOut1.influences();
	auto coupledPort= linked.front()->influences();
	auto id= coupledPort.front()->modelId();
	port_hash[id] = &relayOut1;

	linked = relayOut2.influences();
	coupledPort= linked.front()->influences();
	id= coupledPort.front()->modelId();
	port_hash[id] = &relayOut2;

	linked = relayOut3.influences();
	coupledPort= linked.front()->influences();
	id= coupledPort.front()->modelId();
	port_hash[id] = &relayOut3;

	linked = relayOut4.influences();
	coupledPort= linked.front()->influences();
	id= coupledPort.front()->modelId();
	port_hash[id] = &relayOut4;
	
	this->message_identifier = 0;
	this->updates = 0;
	this->acknowledge = false;

 	// set next transition
 	passivate();
	return *this ;
}

/*******************************************************************
* Function Name: externalFunction
* Description: This method executes when an external event is received.
********************************************************************/
Model &scheduler::externalFunction( const ExternalMessage &msg )
{
#if VERBOSE
	PRINT_TIMES("scheduler_control");
#endif
	//[(!) update common variables]	

	if(msg.port() == queuePort){

		Tuple<Real> packet = Tuple<Real>::from_value(msg.value());
	
		this->number_of_retransmission = packet[1];		
		
		holdIn( AtomicState::active, VTime::Zero );
			
	}else if(msg.port() == protocolPort){

		Tuple<Real> packet = Tuple<Real>::from_value(msg.value());

		int identifier = static_cast<int>(packet[1].value());
		this->updates++;

		std::cout << "model id:"<< identifier << endl;
		
		Real cycle = packet[0];
		std::cout << "duty: " << cycle <<  endl;

		if(this->updates > 4){	
			std::cout << "reset" <<  endl;
			while(!this->priority.empty()){
				this->priority.pop();
				
			}
			this->updates = 0;
		
		}
		if(cycle == Real(1)){
			std::cout << "scheduler updated" <<  std::endl;
			std::cout << identifier <<  std::endl;
			this->priority.push(std::make_pair(this->relay_pdr[identifier],identifier));
		}

	}else{

		std::cout << "checking response..." <<  std::endl;
		Tuple<Real> packet = Tuple<Real>::from_value(msg.value());
		this->message_identifier = static_cast<int>(packet[3].value());
		std::cout << "model id:"<< this->message_identifier << endl;
		this->sent[this->message_identifier]++;

		Real outcome = packet[2];
		this->relay_pdr[this->message_identifier] += static_cast<float>(packet[0].value()) / this->sent[this->message_identifier]; // testear que msg.senderModelId() devuelva id del relay
		this->number_of_retransmission = packet[1];
		this->priority.push(std::make_pair(this->relay_pdr[this->message_identifier],this->message_identifier));	


		if(outcome == Real(0)){
			this->acknowledge = true;

		}else{
			std::cout << "retransmitting..." <<  std::endl;
			this->acknowledge = false;
		}

		holdIn( AtomicState::active, VTime::Zero );

		

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

	 // stays in passive state until an external event occurs;
	passivate();
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

	
	if(Real(this->maxRetransmission) >= this->number_of_retransmission	&& Real(this->priority.size()) > Real(0)){
		
		if(!this->acknowledge){
			std::pair<float,int> selectedRelay =  this->priority.top();
			this->priority.pop();
			std::cout << "selecting..." << endl;
			int outRelay = std::get<1>(selectedRelay);
			std::cout << "selected Relay: " << outRelay << endl; 
			std::cout << "Port: " <<this->port_hash[outRelay]->id() << endl; 
			std::cout << "message transmitted" << endl;
			sendOutput(msg.time(), *this->port_hash[outRelay] , this->number_of_retransmission);
		}else{
			std::cout << "delivered" << endl;
			sendOutput(msg.time(), ack, Real(0) );
		}
			
	}else{
		std::cout << "discarded" << endl;
		sendOutput(msg.time(), ack, Real(0) );
		sendOutput(msg.time(),trow, this->number_of_retransmission);
	}

	
	
	
	
	
	return *this;

}

scheduler::~scheduler()
{
	//TODO: add destruction code here. Free distribution memory, etc. 
}
