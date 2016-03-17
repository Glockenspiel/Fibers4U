#include "include/WaitingTask.h"

namespace fbr{
	WaitingTask::WaitingTask(BaseTask* task, unsigned int count){
		this->task = task;
		this->count = count;
	}

	WaitingTask::WaitingTask(BaseTask* task, unsigned int count, Priority taskPriority){
		this->task = task;
		this->count = count;
		this->taskPriority = taskPriority;
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