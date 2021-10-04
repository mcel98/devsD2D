#include "pmodeladm.h" 
#include "register.h"

#include "due.h" // class ModelName
#include "transmitter.h"
#include "cycleScheduler.h"
#include "scheduler.h"

void register_atomics_on(ParallelModelAdmin &admin)
{
	admin.registerAtomic(NewAtomicFunction< due >(), "due"); // Register custom models
	admin.registerAtomic(NewAtomicFunction< transmitter >(), "transmitter" );
	admin.registerAtomic(NewAtomicFunction< scheduler >(), "scheduler" ); 
	admin.registerAtomic(NewAtomicFunction<  cycleScheduler >(), " cycleScheduler" );
}

