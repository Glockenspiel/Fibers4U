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
	isFree = false;
	std::cout << "running fiber\n";
	currentTask = &task;
	currentTask->run();
}

void Fiber::free(){
	delete currentTask;
	isFree = true;
	*counterPtr--;
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