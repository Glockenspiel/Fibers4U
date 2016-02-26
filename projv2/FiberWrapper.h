#ifndef FIBER_WRAPPER_H
#define FIBER_WRAPPER_H

#include "Fiber.h"
#include <atomic>


class FiberWrapper{
public:
	FiberWrapper();
	~FiberWrapper();
	void run();
	void nextTask(Task& task);
	void switchFiber(Fiber& fiber);
	void close();
	void set(Task& task, Fiber& fiber);
private:
	std::atomic<bool> running = false;
	std::atomic<bool> switched = false;
	std::atomic<bool> setNextTask = false;
	Task* nextTaskPtr;
	Fiber* currentFiber;
};

#endif