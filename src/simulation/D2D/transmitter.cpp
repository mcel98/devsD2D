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
    retransmit(addOutputPort( "retransmit" )),
	ProtocolOut(addOutputPort( "ProtocolOut")),
	ProtocolIn(addInputPort("ProtocolIn")),
    packetPort(addInputPort("packetPort")),
	dist(0.0,1.0)
{
    std::random_device rd;

    rnd.seed(rd());

    distance_to_bs = str2Int( ParallelMainSimulator::Instance().getParameter( description(), "r" ) );

	packet_size = str2Int( ParallelMainSimulator::Instance().getParameter( description(), "size" ) );
    
	packet_split = str2Int( ParallelMainSimulator::Instance().getParameter( description(), "l" ) );
    
	noise = std::stof( ParallelMainSimulator::Instance().getParameter( description(), "noise" ) );

    path_loss_exponent = std::stof( ParallelMainSimulator::Instance().getParameter( description(), "alpha" ) );
    
	transmitter_power = std::stof( ParallelMainSimulator::Instance().getParameter( description(), "Px" ) );;

	mu = std::stof( ParallelMainSimulator::Instance().getParameter( description(), "mu" ) );

	device_density = str2Int( ParallelMainSimulator::Instance().getParameter( description(), "device_density" ) );

	active_devices = std::binomial_distribution<int>(device_density, 0.2);

	devices_maximum_distance = str2Int( ParallelMainSimulator::Instance().getParameter( description(), "r2" ) );

	std::exponential_distribution<float> expGen(mu);
	channel_gain = expGen(rnd);


	




}

/*******************************************************************
* Function Name: initFunction
********************************************************************/
Model &transmitter::initFunction()
{
	
 	// this->sigma = VTime::Inf; // stays in active state until an external event occurs;
 	this->sigma = VTime::Zero; 
	auto linked = retransmit.influences();
	auto relay_id = linked.front()->modelId();
	this->send_info = true;
	std::cout << "transmitter model Id: " << relay_id << endl;
 	// TODO: add init code here. (setting first state, etc)
 	this->interference = 0.0;
 	// set next transition
 	passivate();

	return *this;
}

/*******************************************************************
* Function Name: externalFunction
* Description: This method executes when an external event is received.
********************************************************************/
Model &transmitter::externalFunction( const ExternalMessage &msg )
{
#if VERBOSE
	PRINT_TIMES("received signal");
#endif
	//[(!) update common variables]	
	
	

	if(msg.port() == ProtocolIn){
		this->send_info = true;

		int total = this->active_devices(this->rnd);


		float res = 0;

		std::exponential_distribution<float> expGen(this->mu);
		std::uniform_int_distribution<int> distGen(1, this->devices_maximum_distance );

		for(int i = 0; i<total;i++){

			float h_y_b = expGen(this->rnd);
			float r = static_cast< float >(distGen(this->rnd));
			float alpha = this->path_loss_exponent;

			res += h_y_b * std::pow(r, -1.0 * alpha) * this->transmitter_power;
			
		}


		this->interference = res;

		float power = std::pow( 10, this->transmitter_power / 10. ) / 1000;
		float noise_W = std::pow( 10, -1.0 * this->noise / 10. ) / 1000;
		this->pdr = getPDR(this->channel_gain,this->interference,noise_W,this->path_loss_exponent,power,this->distance_to_bs,this->packet_size,this->packet_split);
		std::cout << this->pdr << endl;
		std::cout << "pdr: " << this->pdr << "channel gain:" << this->channel_gain << " interference: " <<this->interference << " noise: "<< this->noise<< " alpha"<<this->path_loss_exponent << endl;
		
		

	}else{

		this->retransmission = Real::from_value(msg.value());

	}

	holdIn( AtomicState::active, this->sigma);
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
	PRINT_TIMES("transmitter");
#endif
	//TODO: implement the internal function here

	// stays in passive state until an external event occurs;
	this->send_info = false;
	passivate();
	return *this;

}

/*******************************************************************
* Function Name: outputFunction
* Description: This method executes when the TA has expired. After this method the internalFunction is called.
* Output values can be send through output ports
********************************************************************/
Model &transmitter::outputFunction( const CollectMessage &msg )
{

    if(this->send_info){

		std::cout << "pdr sent" << endl;

		auto linked = ProtocolOut.influences();
		auto relay_id = linked.front()->modelId();

		std::vector<Real> vector_out(4,Real(0));
		vector_out.push_back(Real(1));
		vector_out.push_back(Real(relay_id));
		vector_out.push_back(Real(this->pdr));
		Tuple<Real> out_value(&vector_out);
		sendOutput( msg.time(), ProtocolOut, out_value);
	}else{

		double choice = this->dist(rnd);
		auto linked = retransmit.influences();
		auto relay_id = linked.front()->modelId();
		std::cout << "transmitter returns packet's PDR: " << this->pdr << " and choose: " << choice << endl;
		if(choice <= this->pdr){
			std::cout << "sucess" << endl;
			Tuple<Real> hop_value{Real(this->pdr), this->retransmission, Real(relay_id)};
			sendOutput( msg.time(), hop, hop_value);
			Tuple<Real> control_value{Real(pdr), this->retransmission,0,Real(relay_id)};
			sendOutput( msg.time(), retransmit, control_value);	
		}else{
			std::cout << "fail" << endl;
			Tuple<Real> out_value{Real(pdr), this->retransmission,1,Real(relay_id)};
			sendOutput( msg.time(), retransmit, out_value);
		}

	}
	



	
	return *this;

}

float transmitter::getPDR(float channel_gain,float interference,float noise,float path_loss_exponent,float transmitter_power,int distance_to_bs,int packet_size,int packet_split){
	float SINR = 0.0;
	SINR = (transmitter_power * std::pow(distance_to_bs, -1.0 * path_loss_exponent) * channel_gain) / (interference + noise);

	float pb = 0.5 * std::erfc(std::sqrt(SINR)/std::sqrt(2.0));
	float PDR = 1.0;
	for(int i =0; i < ((packet_size / packet_split) + (packet_size % packet_split) ); i++){
		PDR = std::pow((1 - pb), packet_split) * PDR;
	}

	return PDR;
}

transmitter::~transmitter()
{
	//TODO: add destruction code here. Free distribution memory, etc. 
}
