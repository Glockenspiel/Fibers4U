#include "Worker.h"
#include <iostream>
#include "global.h"
#include "Scheduler.h"

Worker::Worker(unsigned int id){
	this->id = id;
}

Worker::~Worker(){}

void Worker::run(){
	running = true;
	while (running.load(std::memory_order_relaxed)){
		if (state.load(std::memory_order_relaxed) == State::prepared){
			currentFiber->runAndFree(*nextTaskPtr);
			setState(State::free);
			Scheduler::workerBeenFreed(this);
		}
	}
}

//switches fiber pointer
void Worker::switchFiber(Fiber& fiber){
	//if nullptr assign it
	if (currentFiber == nullptr){
		currentFiber = &fiber;
		return;
	}
	//no change in fiber pointer
	else if (currentFiber->getID() == fiber.getID()){
		return;
	}
	//assign fiber pointer
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
	setState(State::prepared);
}

//set switch to new fiber
void Worker::set(Fiber& fiber){
	switchFiber(fiber);
	nextTask(*fiber.currentTask);
	setState(State::prepared);
}

//Returns true if this worker has been acquired
//The worker state must be free to acquire it
bool Worker::tryAcquire(){
	if (state.load(std::memory_order_acquire)==State::free){
		setState(State::aquired);
		return true;
	}
	return false;
}

//spin locks until the workers goes to free state and 
//then the fiber changed to acquired state
void Worker::forceAcquire(){
	while (state.load(std::memory_order_relaxed) != free){
		setState(State::aquired);
	}
}

//sets the state of the worker
void Worker::setState(State s){
	state.store(s, std::memory_order_release);
}

unsigned int Worker::getID(){
	return id;
}