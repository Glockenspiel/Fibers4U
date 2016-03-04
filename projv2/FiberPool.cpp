#include "FiberPool.h"

FiberPool::FiberPool(const unsigned int FIBER_COUNT){
	for (unsigned int i = 0; i < FIBER_COUNT; i++)
		fibers.push_back(new Fiber(i));
}

FiberPool::~FiberPool(){
	//fiber pool is static so it doesn't get cleaned up until the program exits
}

//trys to acquire a free fiber, if no free fiber is found a nullptr is returned
Fiber* FiberPool::tryAcquireFreeFiber(){
	for (unsigned int i = 0; i < fibers.size(); i++){
		if (fibers[i]->tryAcquire()) //try acquire
			return fibers[i];
	}
	return nullptr;
}

//returns true if there is any fibers in the low,medium or high queue
bool FiberPool::queueHasNext(){
	return	
		fiberQueueLow.empty() == false ||
		fiberQueueMedium.empty() == false ||
		fiberQueueHigh.empty() == false;
}

//returns the next fiber in queue to be executed, it also pops this fiber out of the queue
Fiber& FiberPool::popNextFiber(){
	Fiber* fiber = nullptr;

	if (fiberQueueHigh.empty() == false){
		fiber = fiberQueueHigh.getPop();
	}
	else if (fiberQueueMedium.empty() == false){
		fiber = fiberQueueMedium.getPop();
	}
	else if ((fiberQueueLow.empty() == false)){
		fiber = fiberQueueLow.getPop();
	}

	return *fiber;
}

//pushes the fiber to its corresponding queue
void FiberPool::pushToQueue(Fiber& fiber){
	switch (fiber.getPriority()){
		case(Priority::high) : fiberQueueHigh.push(&fiber); break;
		case(Priority::medium) : fiberQueueMedium.push(&fiber); break;
		case(Priority::low) : fiberQueueLow.push(&fiber); break;
	}
}