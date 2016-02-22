#ifndef SCHEDULER_H
#define SCHEDULER_H

#include "Task.h"
#include <vector>
#include <atomic>
#include "Fiber.h"

class Scheduler{
public:
	Scheduler(unsigned const int fiberCount);
	~Scheduler();
	void runTask(Task &task);
	
private:
	std::vector<Fiber *> fibers;
	std::vector<Task*> queuedTasks;
	atomic<int> counter=0;
};

#endif