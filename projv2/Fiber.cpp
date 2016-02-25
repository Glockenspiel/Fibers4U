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
	isFree = false;
	*counterPtr += 1;

	//assign the current task and delete the old one
	Task *temp = currentTask;
	currentTask = &task;
	delete temp;
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

	//wait in a spin lock until the fiber is switched out
	spinLock->lock();
}

//atm this ends the thread
void Fiber::switchOut(){
	//wait until locked then unlock
	while (spinLock->getIsLocked() == false){}
	spinLock->unlock();
}

//returns true if the fiber has finished its current task
bool Fiber::isFiberFree(){
	return isFree.load(std::memory_order_relaxed);
}

//call both run() and free()
void Fiber::runAndFree(Task &task){
	run(task);
	free();
	global::writeLock();
	std::cout << "Run and free completed on fiber: " << id << std::endl;
	global::writeUnlock();
}