#include "FiberWrapper.h"
#include <iostream>

FiberWrapper::FiberWrapper(){

}

FiberWrapper::~FiberWrapper(){}

void FiberWrapper::run(){
	running = true;
	while (running.load(std::memory_order_relaxed)){

		//wait until fiber switched and the next task was set
		if (switched.load(std::memory_order_relaxed) && 
			setNextTask.load(std::memory_order_relaxed)){

			//run next task
			currentFiber->runAndFree(*nextTaskPtr);
			
			//currentFiber->switchOut();
			//reset flags
			setNextTask.store(false, std::memory_order_release);
			switched.store(false, std::memory_order_release);

			
		}
	}
}

void FiberWrapper::switchFiber(Fiber& fiber){
	if (currentFiber!=nullptr)
		currentFiber->switchOut();
	currentFiber = &fiber;

	switched.store(true, std::memory_order_release);
}

void FiberWrapper::close(){
	//while (currentFiber->isFiberFree() == false){}
	//currentFiber->switchOut();

	
	currentFiber->waitUntilFree();
	running.store(false, std::memory_order_release);
}

void FiberWrapper::nextTask(Task& task){
	nextTaskPtr = &task;
	setNextTask.store(true, std::memory_order_release);
}

void::FiberWrapper::set(Task& task, Fiber& fiber){
	switchFiber(fiber);
	nextTask(task);
}