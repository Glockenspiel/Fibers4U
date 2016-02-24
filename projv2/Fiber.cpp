#include "Fiber.h"
#include "Scheduler.h"
#include <iostream>

Fiber::Fiber(atomic<int>& counter){
	counterPtr = &counter;
}

Fiber::~Fiber(){
	delete currentTask;
}

void Fiber::run(Task &task){
	*counterPtr += 1;
	isFree = false;

	//global::writeLock();
	//std::cout << "running fiber" << std::endl;
	//global::writeUnlock();

	currentTask = &task;
	currentTask->run();

	//global::writeLock();
	//std::cout << "fiber run completed" << std::endl;
	//global::writeUnlock();
}

void Fiber::free(){
	//delete currentTask;
	//isFree = true;
	int i = *counterPtr;
	*counterPtr -= 1;
	while (i == *counterPtr){}
	global::writeLock();
	std::cout << "Fiber freed" << std::endl;
	global::writeUnlock();

	//std::cout << "fiber::free completed: " << *counterPtr << std::endl;
}

bool Fiber::isFiberFree(){
	//todo check to see if current task pointer can be reused
	//delete currentTask;
	return isFree;
}

void Fiber::runAndFree(Task &task){
	run(task);
	free();
}