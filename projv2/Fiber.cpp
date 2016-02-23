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
	std::cout << "running fiber: " << *counterPtr << std::endl;;
	currentTask = &task;
	currentTask->run();
	std::cout << "fiber::run completed\n";
}

void Fiber::free(){
	//delete currentTask;
	//isFree = true;
	int i = *counterPtr;
	*counterPtr -= 1;
	while (i == *counterPtr){}
	std::cout << "fiber::free completed: " << *counterPtr << std::endl;
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