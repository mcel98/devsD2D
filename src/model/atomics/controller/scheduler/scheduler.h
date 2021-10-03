#ifndef __SCHEDULER_H // cambiar nombre

#define __SCHEDULER_H // cambiar nombre

/** include files **/
#include <random>
#include "atomic.h"  // class Atomic
#include "VTime.h"
#include <map>
#include <vector>
#include <queue>


#define ATOMIC_MODEL_NAME "scheduler" // cambiar nombre

/** forward declarations **/
//TODO: add distribution class declaration here if needed
// Ej: class Distribution ;

/** declarations **/
class scheduler: public Atomic {
	public:
		scheduler( const string &name = ATOMIC_MODEL_NAME ); // Default constructor
		~scheduler(); // Destructor
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
		const Port &queuePort;
		const Port &protocolPort;

		std::vector<Port > relayOut;
		const Port &relayIn;
		Port &trow;

		Port &ack;


		// [(!) declare common variables]
		// Lifetime programmed since the last state transition to the next planned internal transition.
		VTime sigma;
		
		// Time elapsed since the last state transition until now
		VTime elapsed;
		
		// Time remaining to complete the last programmed Lifetime
		VTime timeLeft;

		std::mt19937 rnd;

		float lambda;


        std::map<int, float>  relay_pdr;
		std::vector<float> delivered;
		std::priority_queue<int>  priority;


		bool update;
		int maxRetransmission;
		int message_identifier;

		Real number_of_retransmission;

};	// class scheduler


#endif   //__SCHEDULER_H 
