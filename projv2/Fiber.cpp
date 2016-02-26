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

	setState(freed);
}


void Fiber::setTask(Task &task){
	Task *temp = currentTask;
	currentTask = &task;
	//can uncomment this later. using this for testing with fiber wrapper
	//delete temp;
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
	waitForState(freed);
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

bool Fiber::inState(Fiber::State s){
	return state.load(std::memory_order_relaxed) == s;
}