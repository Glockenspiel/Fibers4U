#ifndef WAITING_TASK_H
#define WAITING_TASK_H

#include "BaseTask.h"
#include "Fiber.h"

class WaitingTask{
public:
	//waiting task with low priority
	WaitingTask(BaseTask* task, unsigned int count);
	//waiting task with a given prioirty
	WaitingTask(BaseTask* task, unsigned int count, Priority taskPriority);
	~WaitingTask();

	//returns the waiting count
	int getWaitingCount();

	//returns the task
	BaseTask* getTask();

	//returns the priority
	Priority getPriority();
private:
	BaseTask* task;
	int count;
	priority::Priority taskPriority = priority::high;
};

#endif