#include "pmodeladm.h" 
#include "register.h"

#include "cycleScheduler.h" // class ModelName

void register_atomics_on(ParallelModelAdmin &admin)
{
	admin.registerAtomic(NewAtomicFunction< cycleScheduler >(), ATOMIC_MODEL_NAME); // Register custom models
}

