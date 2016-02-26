#include "FiberWrapper.h"
#include <iostream>

FiberWrapper::FiberWrapper(){

}

FiberWrapper::~FiberWrapper(){}

void FiberWrapper::run(){
	running = true;
	while (running.load(std::memory_order_relaxed)){
		currentFiber->runAndFree(*nextTaskPtr);
	}
}

void FiberWrapper::switchFiber(Fiber& fiber){
	if (currentFiber != nullptr)
		currentFiber->waitUntilFree();
	currentFiber = &fiber;
}

void FiberWrapper::close(){
	//wait for current fiber to be set
	while (currentFiber == nullptr){}

	currentFiber->waitUntilFree();
	running.store(false, std::memory_order_release);
}

void FiberWrapper::nextTask(Task& task){
	nextTaskPtr = &task;
}

void::FiberWrapper::set(Task& task, Fiber& fiber){
	switchFiber(fiber);
	nextTask(task);
}