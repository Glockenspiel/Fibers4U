#include "Worker.h"
#include <iostream>

Worker::Worker(){

}

Worker::~Worker(){}

void Worker::run(){
	running = true;
	while (running.load(std::memory_order_relaxed)){
		currentFiber->runAndFree(*nextTaskPtr);
	}
}

void Worker::switchFiber(Fiber& fiber){
	if (currentFiber != nullptr)
		currentFiber->waitUntilFree();
	currentFiber = &fiber;
}

void Worker::close(){
	//wait for current fiber to be set
	while (currentFiber == nullptr){}

	currentFiber->waitUntilFree();
	running.store(false, std::memory_order_release);
}

void Worker::nextTask(Task& task){
	nextTaskPtr = &task;
}

void Worker::set(Task& task, Fiber& fiber){
	switchFiber(fiber);
	nextTask(task);
}