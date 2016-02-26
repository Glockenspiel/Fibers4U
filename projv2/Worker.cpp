#include "Worker.h"
#include <iostream>

Worker::Worker(){

}

Worker::~Worker(){}

void Worker::run(){
	running = true;
	while (running.load(std::memory_order_relaxed)){
		if (currentFiber->inState(Fiber::State::prepared))
			currentFiber->runAndFree(*nextTaskPtr);
	}
}

//switches fiber
void Worker::switchFiber(Fiber& fiber){
	if (currentFiber == nullptr){
		currentFiber = &fiber;
		return;
	}
	//no change in fiber
	else if (currentFiber->getID() == fiber.getID()){
		return;
	}
	else
	{
		currentFiber = &fiber;
	}
}

//ends worker
void Worker::close(){
	//wait for current fiber to be set
	while (currentFiber == nullptr){}

	currentFiber->waitUntilFree();
	running.store(false, std::memory_order_release);
}

//sets next task
void Worker::nextTask(Task& task){
	nextTaskPtr = &task;
}

//set task and fiber
void Worker::set(Task& task, Fiber& fiber){
	switchFiber(fiber);
	nextTask(task);
}