#ifndef FIBER_POOL_H
#define FIBER_POOL_H

#include "Fiber.h"
#include "con_queue.h"
#include "con_vector.h"
#include <vector>

namespace fbr{
	//atomic spinlock flag, used for accessing the queue atomically
	static std::atomic_flag queueLock = ATOMIC_FLAG_INIT;

	class FiberPool{
		friend class Scheduler;
	public:
		FiberPool(const unsigned int FIBER_COUNT);
		~FiberPool();

		//trys to acquire a fiber which is free, returns nullptr if no free fibers
		Fiber* tryAcquireFreeFiber();

		//returns true if there is an element in one of the queues
		bool queueHasNext();

		//pops the next highest priority fiber off the queues
		Fiber& popNextFiber();

		//pushes a fiber onto its relevant queue. (depending on the fibers priority)
		void pushToQueue(Fiber& fiber);

	private:
		//list of all the fibers available
		con_vector<Fiber *> fibers;

		//queues for fibers not yet set to workers
		con_queue<Fiber *> fiberQueueLow;
		con_queue<Fiber *> fiberQueueMedium;
		con_queue<Fiber *> fiberQueueHigh;
	};
}
#endif