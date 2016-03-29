#ifndef WAITING_TASK_H
#define WAITING_TASK_H

#include "BaseTask.h"
#include "Fiber.h"
#include "con_data_struct\Counter.h"

namespace fbr{
	class WaitingTask{
	public:
		//waiting task with a given prioirty
		WaitingTask(Counter* waitingCtr, BaseTask* task, unsigned int count, 
			Priority taskPriority, Counter* taskCtr);
		~WaitingTask();

		//returns the waiting count
		unsigned int getWaitingCount();

		//returns the task
		BaseTask* getTask();

		//returns the priority
		Priority getPriority();

		//get task counter
		Counter* getTaskCounter();

		//returns true if waiting counter is equal or below the count
		bool isReadyToRun();

	private:
		BaseTask* task;
		unsigned int count;
		priority::Priority taskPriority = priority::high;
		Counter* waitingCtr;
		Counter* taskCtr;
	};
}

#endif