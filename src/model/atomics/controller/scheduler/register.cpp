#include "pmodeladm.h" 
#include "register.h"

#include "scheduler.h" // class ModelName

void register_atomics_on(ParallelModelAdmin &admin)
{
	admin.registerAtomic(NewAtomicFunction< scheduler >(), ATOMIC_MODEL_NAME); // Register custom models
}

