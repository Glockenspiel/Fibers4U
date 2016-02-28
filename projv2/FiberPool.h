#ifndef FIBER_POOL_H
#define FIBER_POOL_H

#include "Fiber.h"
#include <queue>
#include <vector>

using std::queue;
using std::vector;

//static queue for fibers not yet set to workers
static queue<Fiber *> fiberQueue;

//atomic spinlock flag, used for accessing the queue atomically
static std::atomic_flag queueLock = ATOMIC_FLAG_INIT;

class FiberPool{
	friend class Scheduler;
public:
	FiberPool(const unsigned int FIBER_COUNT, std::atomic<int>& counter);
	~FiberPool();

	Fiber* acquireFreeFiber();

private:
	std::vector<Fiber *> fibers;
	queue<Fiber *>* qPtr = &fiberQueue;
};

#endif
