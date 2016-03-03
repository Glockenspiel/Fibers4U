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

//pushes the fiber to its corresponding queue
void FiberPool::pushToQueue(Fiber& fiber){
	switch (fiber.getPriority()){
		case(Priority::high) : fiberQueueHigh.push(&fiber); break;
		case(Priority::medium) : fiberQueueMedium.push(&fiber); break;
		case(Priority::low) : fiberQueueLow.push(&fiber); break;
	}

	//while (counterLock.test_and_set(std::memory_order_seq_cst));
	//realCounter += 1;
	//counterLock.clear(std::memory_order_seq_cst);
}

/*
int FiberPool::queueCount(){
	//return fiberQueueHigh.size() + fiberQueueMedium.size() + fiberQueueLow.size();
	int a;
	while (counterLock.test_and_set(std::memory_order_seq_cst));
	a = realCounter;
	counterLock.clear(std::memory_order_seq_cst);

	return a;
}
*/
/*
void FiberPool::workerStarted(int a){
	while (counterLock.test_and_set(std::memory_order_seq_cst));
	realCounter += a;
	counterLock.clear(std::memory_order_seq_cst);
}
*/