#ifndef _TRANSMITTER_H_ // cambiar nombre

#define _TRANSMITTER_H_ // cambiar nombre

/** include files **/
#include <random>
#include "atomic.h"  // class Atomic
#include "VTime.h"
#include <math.h>


#define ATOMIC_MODEL_NAME "transmitter" // cambiar nombre

/** forward declarations **/
//TODO: add distribution class declaration here if needed
// Ej: class Distribution ;

/** declarations **/


class transmitter: public Atomic {
	public:
		transmitter( const string &name = ATOMIC_MODEL_NAME ); // Default constructor
		~transmitter(); // Destructor
		virtual string className() const {return ATOMIC_MODEL_NAME;}
	
	protected:
		Model &initFunction();	
		Model &externalFunction( const ExternalMessage & );
		Model &internalFunction( const InternalMessage & );
		Model &outputFunction( const CollectMessage & );
	
	private:
		// [(!) TODO: declare ports, distributions and other private varibles here]
		/***********      Example declarations   **********************************/
		// const Port &in;	// this is an input port named 'in'
		// Port &out ;   	// this is an output port named 'out'
		// Distribution *dist ;
		// Distribution &distribution()	{ return *dist; }
		/**************************************************************************/
        const Port &packetPort;
        Port &protocol;
        Port &hop;
	
		// [(!) declare common variables]
		// Lifetime programmed since the last state transition to the next planned internal transition.
		VTime sigma;
		
		// Time elapsed since the last state transition until now
		VTime elapsed;
		
		// Time remaining to complete the last programmed Lifetime
		VTime timeLeft;


		float getPDR(float channel_gain, float interference,float noise,float path_loss_exponent,float transmitter_power,int distance_to_bs,int packet_size,int packet_split);

		std::mt19937 rnd;
        std::uniform_real_distribution<double> dist;
		std::binomial_distribution<int>active_devices;
		

		float mu;
		float channel_gain;
        int distance_to_bs;
		int devices_maximum_distance;
		int packet_size;
        int packet_split;
        float noise;
        float path_loss_exponent;
        float transmitter_power;
		float interference;
		int device_density;
		Real retransmission;

	
};	// class relay


#endif   //__TRANSMITTER_H 
