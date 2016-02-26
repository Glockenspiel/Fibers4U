#ifndef WORKER_H
#define WORKER_H

#include "Fiber.h"
#include <atomic>


class Worker{
public:
	Worker();
	~Worker();
	void run();
	void nextTask(Task& task);
	void switchFiber(Fiber& fiber);
	void close();
	void set(Task& task, Fiber& fiber);
private:
	std::atomic<bool> running = false;
	Task* nextTaskPtr;
	Fiber* currentFiber;
};

#endif