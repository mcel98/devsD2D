/** include files **/
#include <random>
#include <string>
#include <stdlib.h>
#include <time.h>


#include "message.h" // class ExternalMessage, InternalMessage
#include "parsimu.h" // ParallelMainSimulator::Instance().getParameter( ... )
#include "real.h"
#include "tuple_value.h"
#include "strutil.h"

#include "transmitter.h" // Cambiar nombre, base header


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
transmitter::transmitter( const std::string &name ) : 
	Atomic( name ),
    hop(addOutputPort( "hop" )),
    protocol(addOutputPort( "protocol" )),
    packetPort(addInputPort("packetPort")),
	dist(0.0,1.0)
{
    std::mt19937::result_type seed = time(nullptr);

    rnd.seed(seed);

    distance_to_bs = str2Int( ParallelMainSimulator::Instance().getParameter( description(), "r" ) );

	packet_size = str2Int( ParallelMainSimulator::Instance().getParameter( description(), "size" ) );
    
	packet_split = str2Int( ParallelMainSimulator::Instance().getParameter( description(), "l" ) );
    
	noise = std::stof( ParallelMainSimulator::Instance().getParameter( description(), "noise" ) );

    path_loss_exponent = std::stof( ParallelMainSimulator::Instance().getParameter( description(), "alpha" ) );
    
	transmitter_power = std::stof( ParallelMainSimulator::Instance().getParameter( description(), "Px" ) );;

	mu = std::stof( ParallelMainSimulator::Instance().getParameter( description(), "mu" ) );

	active_devices = std::binomial_distribution<int>(device_density, 0.2);

	delay = str2Int( ParallelMainSimulator::Instance().getParameter( description(), "tau" ) );

	std::exponential_distribution<float> expGen(mu);
	channel_gain = expGen(rnd);

	VTime frequency_time = VTime(0,0,0,delay);

	




}

/*******************************************************************
* Function Name: initFunction
********************************************************************/
Model &transmitter::initFunction()
{
	// [(!) Initialize common variables]
	this->elapsed  = VTime::Zero;
 	this->timeLeft = VTime::Inf;
 	// this->sigma = VTime::Inf; // stays in active state until an external event occurs;
 	this->sigma    = VTime::Inf; // force an internal transition in t=0;

 	// TODO: add init code here. (setting first state, etc)
 	this->interference = 0;
 	// set next transition
 	holdIn( AtomicState::passive, this->sigma  ) ;
	return *this ;
}

/*******************************************************************
* Function Name: externalFunction
* Description: This method executes when an external event is received.
********************************************************************/
Model &transmitter::externalFunction( const ExternalMessage &msg )
{
#if VERBOSE
	PRINT_TIMES("dext");
#endif
	//[(!) update common variables]	
	this->sigma    = nextChange();	
	this->elapsed  = msg.time()-lastChange();	
 	this->timeLeft = this->sigma - this->elapsed; 

	int total = this->active_devices(this->rnd);

	int res = 0;

	std::exponential_distribution<float> expGen(this->mu);
	std::uniform_int_distribution<int> distGen(0, this->device_density);

	for(int i = 0; i<total;i++){

		float h_y_b = expGen(this->rnd);
		float r = static_cast< float >(distGen(this->rnd));

		res += h_y_b + std::pow(r, -1.0 * this->path_loss_exponent);
		
	}

	this->interference = res;
	this-> retransmission = Tuple<Real>::from_value(msg.value())[2] + Real(1);

	holdIn( AtomicState::active, this->frequency_time);
	return *this ;
}

/*******************************************************************
* Function Name: internalFunction
* Description: This method executes when the TA has expired, right after the outputFunction has finished.
* The new state and TA should be set.
********************************************************************/
Model &transmitter::internalFunction( const InternalMessage &msg )
{
#if VERBOSE
	PRINT_TIMES("dint");
#endif
	//TODO: implement the internal function here

	this->sigma = VTime::Inf; // stays in passive state until an external event occurs;

	holdIn( AtomicState::passive, this->sigma );
	return *this;

}

/*******************************************************************
* Function Name: outputFunction
* Description: This method executes when the TA has expired. After this method the internalFunction is called.
* Output values can be send through output ports
********************************************************************/
Model &transmitter::outputFunction( const CollectMessage &msg )
{

    float pdr = getPDR(this->channel_gain,this->interference,this->noise,this->path_loss_exponent,this->transmitter_power,this->distance_to_bs,this->packet_size,this->packet_split);

	double choice = this->dist(rnd);
	if(choice <= pdr){
		sendOutput( msg.time(), hop, Real(pdr));
		Tuple<Real> out_value{Real(pdr),0, this->retransmission};
		sendOutput( msg.time(), protocol, out_value);	
	}else{
		Tuple<Real> out_value{Real(pdr), 1, this->retransmission};
		sendOutput( msg.time(), protocol, out_value);
	}


	
	return *this;

}

float &transmitter::getPDR(float channel_gain,float interference,float noise,float path_loss_exponent,float transmitter_power,int distance_to_bs,int packet_size,int packet_split){
	float pdr = 0.0;
	pdr = (transmitter_power * std::pow(distance_to_bs, path_loss_exponent) * channel_gain) / (interference + noise);

	float PDR = 0.0;
	for(int i =0; i < packet_size / packet_split; i++){
		PDR += std::pow((1 - pdr), packet_split);
	}

	return PDR;
}

transmitter::~transmitter()
{
	//TODO: add destruction code here. Free distribution memory, etc. 
}
