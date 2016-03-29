#include "include/WaitingTask.h"

namespace fbr{

	WaitingTask::WaitingTask(Counter* ctr, BaseTask* task, unsigned int count,
		Priority taskPriority, Counter* taskCtr){
		this->task = task;
		this->count = count;
		this->taskPriority = taskPriority;
		this->ctr = ctr;
		this->taskCtr = taskCtr;
	}

	WaitingTask::~WaitingTask(){}

	//returns the waiting count
	unsigned int WaitingTask::getWaitingCount(){
		return count;
	}

	//returns the task
	BaseTask* WaitingTask::getTask(){
		return task;
	}

	//returns the priority
	Priority WaitingTask::getPriority(){
		return taskPriority;
	}
}