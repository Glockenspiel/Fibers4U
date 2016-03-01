#include "WaitingTask.h"

WaitingTask::WaitingTask(BaseTask* task, int count){
	this->task = task;
	this->count = count;
}

WaitingTask::WaitingTask(BaseTask* task, int count, Priority taskPriority){
	this->task = task;
	this->count = count;
	this->taskPriority = taskPriority;
}

WaitingTask::~WaitingTask(){}


int WaitingTask::getWaitingCount(){
	return count;
}

BaseTask* WaitingTask::getTask(){
	return task;
}

Priority WaitingTask::getPriority(){
	return taskPriority;
}