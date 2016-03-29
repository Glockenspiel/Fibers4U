#include "include/WaitingTask.h"

namespace fbr{

	WaitingTask::WaitingTask(Counter* waitingCtr, BaseTask* task, unsigned int count,
		Priority taskPriority, Counter* taskCtr){
		this->task = task;
		this->count = count;
		this->taskPriority = taskPriority;
		this->waitingCtr = waitingCtr;
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

	Counter* WaitingTask::getTaskCounter(){
		return taskCtr;
	}

	bool WaitingTask::isReadyToRun(){
		return waitingCtr->get() <= count;
	}
}