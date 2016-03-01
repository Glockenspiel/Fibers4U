#ifndef WAITING_TASK_H
#define WAITING_TASK_H

#include "BaseTask.h"
#include "Fiber.h"

class WaitingTask{
public:
	WaitingTask(BaseTask* task, int count);
	WaitingTask(BaseTask* task, int count, Priority taskPriority);
	~WaitingTask();
	int getWaitingCount();
	BaseTask* getTask();
	Priority getPriority();
private:
	BaseTask* task;
	int count;
	priority::Priority taskPriority = priority::high;
};

#endif