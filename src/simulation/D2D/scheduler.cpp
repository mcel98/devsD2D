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
	protocolIn(addInputPort("protocolIn")),
	ack(addOutputPort("ack")),
	relayIn(addInputPort("relayIn")),
	relayOut1(addOutputPort("relay1Out")),
	relayOut2(addOutputPort("relay2Out")),
	relayOut3(addOutputPort("relay3Out")),
	relayOut4(addOutputPort("relay4Out")),
	protocolOut(addOutputPort("protocolOut")),
	priority(4,0)
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
	this->wait_for_info = false;


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
		this->acknowledge = false;
		this->wait_for_info = true;
		this->updates = 0;

		std::cout << "scheduler ask for update" << endl;

		holdIn( AtomicState::active, VTime::Zero );


		

			
	}else if(msg.port() == protocolIn){
		
		Tuple<Real> packet = Tuple<Real>::from_value(msg.value());
		int update_from = static_cast<int>(packet[4].value());
		int relay_identifier = static_cast<int>(packet[5].value());

		std::cout << "model id:"<< relay_identifier << endl;
		if(update_from == 0){
			std::vector<Real> cycle;
			for(int i= 0; i<4;i++){

				cycle.push_back(packet[i].value());
				std::cout << "duty: " << cycle[i] <<  endl;

			}
			

			
			
			duty_cycle_window[relay_identifier] = cycle;
		}else{
			float pdr = static_cast<float>(packet[6].value());
			this->relay_pdr[relay_identifier] = pdr;
			std::cout << "model id:"<< relay_identifier << endl;
			std::cout << "pdr: " << pdr <<  endl;
		}

		this->updates++;
		std::cout << this->updates << endl;
		if(this->updates == 8){
			this->choose_priority(this->relay_pdr, this->duty_cycle_window , this->priority);
			std::cout << "scheduler updated" <<  std::endl;
			this->wait_for_info = false;
			holdIn( AtomicState::active, VTime::Zero );
		}	
		
		
	}else{

		std::cout << "checking response..." <<  std::endl;
		Tuple<Real> packet = Tuple<Real>::from_value(msg.value());
		this->message_identifier = static_cast<int>(packet[3].value());
		std::cout << "model id:"<< this->message_identifier << endl;
		this->sent[this->message_identifier]++;
		Real outcome = packet[2];
		this->number_of_retransmission = packet[1];

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

	passivate();
	
	 // stays in passive state until an external event occurs;
	
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
	if(this->wait_for_info){

		if(this->updates == 0){
			sendOutput(msg.time(), protocolOut, Real(0) );
		}

	}else{	
			int outRelay =  this->priority[static_cast<int>(this->number_of_retransmission.value())];
			while(outRelay == 0 &&  this->priority.size() > static_cast<int>(this->number_of_retransmission.value()) ){
				this->number_of_retransmission = this->number_of_retransmission + Real(1);
				outRelay = this->priority[static_cast<int>(this->number_of_retransmission.value())];

			}

			if(Real(this->maxRetransmission) >= this->number_of_retransmission){
			
				if(!this->acknowledge){
					this->number_of_retransmission = this->number_of_retransmission + Real(1);
					std::cout << "selecting..." << endl;
					std::cout << "selected Relay: " << outRelay << endl; 
					std::cout << "Port: " <<this->port_hash[outRelay]->id() << endl; 
					std::cout << "message transmitted" << endl;
					sendOutput(msg.time(), *this->port_hash[outRelay] , this->number_of_retransmission);
					
				}else{
					std::cout << "delivered/listening for package" << endl;
					sendOutput(msg.time(), ack, Real(0) );
				}

			}else{
				Tuple<Real> trow_info{this->relay_pdr[this->message_identifier], this->number_of_retransmission,  this->message_identifier};
				sendOutput(msg.time(), ack, Real(0) );
				sendOutput(msg.time(),trow, trow_info);
			}


	}
	


	
	
	
	
	
	return *this;

}

scheduler::~scheduler()
{
	//TODO: add destruction code here. Free distribution memory, etc. 
}


void scheduler::choose_priority(std::map<int, float> pdr, std::map<int, std::vector<Real> > window, std::vector< int > &res){
	//TODO: si un relay es elegido y ya habia se utilizado para un frame anterior descartar
	std::vector<bool> candidate(4,true);
	for (std::map<int,std::vector<Real> >::iterator it=window.begin(); it!=window.end(); ++it){
		std::vector<Real> relay_window = it->second;
		float relay_pdr = pdr[it->first];
		int has_n_cycles = 0;
		for(int i=0; i<4; i++){
			float max_pdr = res[i] != 0? pdr[res[i]]:0;			
			bool cycle = Real(1) == relay_window[i]? true:false; //0000 0001 && 0000 0001
		
			if(cycle && ( (max_pdr < relay_pdr && has_n_cycles == 0) || (candidate[i] &&  has_n_cycles == 0) || res[i] == 0)){
				res[i]=it->first;
				has_n_cycles++;
				candidate[i] = false;
				if(has_n_cycles > 1){

					candidate[i] = true;

				}
			}
		
		}

	}
	return;
}
