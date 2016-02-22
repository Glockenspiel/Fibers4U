#include "Fiber.h"
#include "Scheduler.h"

Fiber::Fiber(atomic<int>& counter){
	counterPtr = &counter;
}

Fiber::~Fiber(){
	delete currentTask;
}

void Fiber::run(Task &task){
	isFree = false;
	currentTask = &task;
	currentTask->run();
}

void Fiber::free(){
	delete currentTask;
	isFree = true;
	*counterPtr--;
}

bool Fiber::isFiberFree(){
	return isFree;
}

void Fiber::runAndFree(Task &task){
	run(task);
	free();
}