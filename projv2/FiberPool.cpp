#include "FiberPool.h"

FiberPool::FiberPool(const unsigned int FIBER_COUNT, std::atomic<int>& counter){
	for (unsigned int i = 0; i < FIBER_COUNT; i++)
		fibers.push_back(new Fiber(counter, i));
}

FiberPool::~FiberPool(){
	//delete all fibers
	for (Fiber* f : fibers)
		delete f;
	fibers.clear();

	while (fiberQueue.empty() == false){
		delete fiberQueue.front();
		fiberQueue.pop();
	}
}

Fiber* FiberPool::acquireFreeFiber(){
	for (unsigned int i = 0; i < fibers.size(); i++){
		if (fibers[i]->tryAcquire()) //try acquire
			return fibers[i];
	}
	return nullptr;
}

