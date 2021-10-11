#ifndef _STATS_H_ // cambiar nombre

#define _STATS_H_ // cambiar nombre

/** include files **/
#include <random>
#include "atomic.h"  // class Atomic
#include "VTime.h"
#include <map>
#include <math.h>
#include <vector>

#define ATOMIC_MODEL_NAME "stats" // cambiar nombre

/** forward declarations **/
//TODO: add distribution class declaration here if needed
// Ej: class Distribution ;

/** declarations **/
class stats: public Atomic {
	public:
		stats( const string &name = ATOMIC_MODEL_NAME ); // Default constructor
		~stats(); // Destructor
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
        const Port &success;
		const Port &fail;
		Port& out;
	
	
		// [(!) declare common variables]
		// Lifetime programmed since the last state transition to the next planned internal transition.
		VTime sigma;
		
		// Time elapsed since the last state transition until now
		VTime elapsed;
		
		// Time remaining to complete the last programmed Lifetime
		VTime timeLeft;

		bool type;

		double failed;
		Tuple<Real> delivered;

	
};	// class ModelTemplate


#endif   //__stats_H 
