#include "Fiber.h"
#include "Scheduler.h"
#include <iostream>

using namespace priority;

Fiber::Fiber(atomic<int>& counter, unsigned short id){
	counterPtr = &counter;
	spinLock = new SpinLock();
	this->id = id;
}

Fiber::~Fiber(){
	delete currentTask;
	delete spinLock;
}

//runs the current task
void Fiber::run(){
	waitForState(prepared);
	global::writeLock();
	std::cout << "Exe..." << id << std::endl;
	global::writeUnlock();

	currentTask->run();

	global::writeLock();
	std::cout << "Com..." << id << std::endl;
	global::writeUnlock();
}

//alert the scheduler that this task has completed and is ready for reallocation
void Fiber::freeTask(){
	*counterPtr -= 1;

	global::writeLock();
	std::cout << "freed:"<< id << std::endl;
	global::writeUnlock();

	setState(free);

}

//sets the current task of the fiber and the fibers priority
void Fiber::setTask(BaseTask* task, Priority p){
	currentPriority = p;

	if (task == currentTask) return;

	BaseTask *temp = currentTask;
	currentTask = task;

	delete temp;
}

//runs the current task and frees it
void Fiber::runAndFree(){
	run();
	freeTask();
}

unsigned int Fiber::getID(){
	return id;
}

//wait until fiber is freed and set state to prepared
void Fiber::setPrepared(){
	waitForState(acquired);
	state = prepared;
}

void Fiber::waitForState(State s){
	while (state.load(std::memory_order_relaxed) != s){}
}

void Fiber::setState(State s){
	state.store(s, std::memory_order_release);
}

void Fiber::waitUntilFree(){
	while (state.load(std::memory_order_relaxed) != free){}
}

bool Fiber::inState(Fiber::State s){
	return state.load(std::memory_order_relaxed) == s;
}

//trys to change state from freed to aquire, returns true if state change was a success
bool Fiber::tryAcquire(){
	if (state.load(std::memory_order_relaxed) == free){
		setState(acquired);
		*counterPtr += 1;
		return true;
	}
	return false;
}