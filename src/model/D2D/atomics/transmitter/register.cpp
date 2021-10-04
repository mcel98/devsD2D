#include "pmodeladm.h" 
#include "register.h"

#include "transmitter.h" // class ModelName

void register_atomics_on(ParallelModelAdmin &admin)
{
	admin.registerAtomic(NewAtomicFunction< transmitter >(), ATOMIC_MODEL_NAME); // Register custom models
}

