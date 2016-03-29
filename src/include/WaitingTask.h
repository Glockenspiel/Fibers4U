#ifndef WAITING_TASK_H
#define WAITING_TASK_H

#include "BaseTask.h"
#include "Fiber.h"
#include "con_data_struct\Counter.h"

namespace fbr{
	class WaitingTask{
	public:
		//waiting task with low priority
		WaitingTask(BaseTask* task, unsigned int count);
		//waiting task with a given prioirty
		WaitingTask(Counter* ctr, BaseTask* task, unsigned int count, 
			Priority taskPriority, Counter* taskCtr);
		~WaitingTask();

		//returns the waiting count
		unsigned int getWaitingCount();

		//returns the task
		BaseTask* getTask();

		//returns the priority
		Priority getPriority();

		//get task counter
		Counter& getTaskCounter();
	private:
		BaseTask* task;
		unsigned int count;
		priority::Priority taskPriority = priority::high;
		Counter* ctr;
		Counter* taskCtr;
	};
}

#endif