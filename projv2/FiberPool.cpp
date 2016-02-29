#include "FiberPool.h"

FiberPool::FiberPool(const unsigned int FIBER_COUNT, std::atomic<int>& counter){
	for (unsigned int i = 0; i < FIBER_COUNT; i++)
		fibers.push_back(new Fiber(counter, i));
}

FiberPool::~FiberPool(){
	//fiber pool is static so it doesn't get cleaned up until the program exits
}

Fiber* FiberPool::acquireFreeFiber(){
	for (unsigned int i = 0; i < fibers.size(); i++){
		if (fibers[i]->tryAcquire()) //try acquire
			return fibers[i];
	}
	return nullptr;
}

bool FiberPool::queueHasNext(){
	return	
		fiberQueueLow.empty() == false ||
		fiberQueueMedium.empty() == false ||
		fiberQueueHigh.empty() == false;
}

Fiber& FiberPool::popNextFiber(){
	Fiber* fiber = nullptr;

	if (fiberQueueHigh.empty() == false){
		fiber = fiberQueueHigh.front();
		fiberQueueHigh.pop();
	}
	else if (fiberQueueMedium.empty() == false){
		fiber = fiberQueueMedium.front();
		fiberQueueMedium.pop();
	}
	else if ((fiberQueueLow.empty() == false)){
		fiber = fiberQueueLow.front();
		fiberQueueLow.pop();
	}

	return *fiber;
}

void FiberPool::pushToQueue(Fiber& fiber){
	switch (fiber.getPriority()){
		case(Priority::high) : fiberQueueHigh.push(&fiber); break;
		case(Priority::medium) : fiberQueueMedium.push(&fiber); break;
		case(Priority::low) : fiberQueueLow.push(&fiber); break;
	}
}