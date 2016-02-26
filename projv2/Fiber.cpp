#include "Fiber.h"
#include "Scheduler.h"
#include <iostream>

Fiber::Fiber(atomic<int>& counter, unsigned short id){
	counterPtr = &counter;
	spinLock = new SpinLock();
	this->id = id;
}

Fiber::~Fiber(){
	delete currentTask;
}

//run the task given
void Fiber::run(Task &task){
	//wait prepared state
	waitForState(prepared);
	setState(running);
	isFree = false;
	*counterPtr += 1;

	//assign the current task and delete the old one
	setTask(task);
	currentTask->run();
}

//alert the scheduler that this task has completed and is ready for reallocation
void Fiber::free(){
	int i = *counterPtr;
	*counterPtr -= 1;
	while (i == *counterPtr){}
	global::writeLock();
	std::cout << "Fiber freed" << std::endl;
	global::writeUnlock();

	isFree.store(true, std::memory_order_release);
	setState(freed);
	//wait in a spin lock until the fiber is switched out
	//spinLock->lock();
}

//atm this ends the thread
void Fiber::switchOut(){
	//wait until locked then unlock
	//while (spinLock->getIsLocked() == false){}
	spinLock->unlock();
}

void Fiber::setTask(Task &task){
	Task *temp = currentTask;
	currentTask = &task;
	//can uncomment this later. using this for testing with fiber wrapper
	//delete temp;
}

//returns true if the fiber has finished its current task
bool Fiber::isFiberFree(){
	return isFree.load(std::memory_order_relaxed);
}

//call both run() and free()
void Fiber::runAndFree(Task &task){
	run(task);
	free();
}

unsigned int Fiber::getID(){
	return id;
}

//wait until fiber is freed and set state to prepared
void Fiber::setPrepared(){
	//wait free state
	while (state == running || state == prepared){}

	state = prepared;
}

void Fiber::waitForState(State s){
	while (state.load(std::memory_order_relaxed) != s){}
}

void Fiber::setState(State s){
	state.store(s, std::memory_order_release);
}

void Fiber::waitUntilFree(){
	while (state.load(std::memory_order_relaxed) != freed){}
}