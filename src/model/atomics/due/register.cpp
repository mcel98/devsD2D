#include "pmodeladm.h" 
#include "register.h"

#include "due.h" // class ModelName

void register_atomics_on(ParallelModelAdmin &admin)
{
	admin.registerAtomic(NewAtomicFunction< due >(), ATOMIC_MODEL_NAME); // Register custom models
}

