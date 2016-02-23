#include "Scheduler.h"
#include <iostream>
#include <functional>
#include "Timer.h"

using namespace std::placeholders;




Scheduler::Scheduler(unsigned const int FIBER_COUNT, unsigned const int THREAD_COUNT){
	N_FIBER_PTR = &FIBER_COUNT;
	N_THREAD_PTR = &THREAD_COUNT;

	if (FIBER_COUNT < THREAD_COUNT){
		std::cout << "Scheduler not started!" << std::endl <<
			"Need more fibers: fibers >= threads" << std::endl <<
			"Thread count: " << THREAD_COUNT << std::endl <<
			"Fiber count: " << FIBER_COUNT << std::endl;

		isConstructed = false;
		return;
	}

	for (unsigned int i = 0; i < FIBER_COUNT; i++){
		fibers.push_back(new Fiber(counter));
	}

	for (unsigned int i = 0; i < THREAD_COUNT; i++){
		//create the spin lock
		spinLocks.push_back(new SpinLock());
		std::function<void()> fn = std::bind(&SpinLock::lock, spinLocks[0]);

		//decrement counter as std::bind creates a decayed copy
		//while (counter == 0){}
		//counter -= 1;

		tasks.push_back(new Task(fn));
		thread *t = new thread(&Fiber::runAndFree, fibers[0], *tasks[i]);
		threads.push_back(t);
	}


	std::cout << "begin counter:" << counter << std::endl;
	
	

	//wait till all spin lock starts
	for(unsigned int index=0; index<THREAD_COUNT; index++){
		if (spinLocks[index]->getIsLocked()){
			spinLocks[index]->unlock();
		}
	}
	
	std::cout << "constructor end, counter:" << counter << std::endl;
}

Scheduler::~Scheduler(){
	//desrtuct fibers
	for (std::vector< Fiber* >::iterator it = fibers.begin(); it != fibers.end(); ++it)
		delete (*it);
	fibers.clear();

	//destruct queued tasks
	for (std::vector< Task* >::iterator it = queuedTasks.begin(); it != queuedTasks.end(); ++it)
		delete (*it);
	queuedTasks.clear();
}

void Scheduler::runTask(Task &task){
	//try find an available fiber
	bool flag = true;
	for (unsigned int i = 0; i<fibers.size() && flag; i++)
		if (fibers[i]->isFiberFree()){
			fibers[i]->runAndFree(task);
			flag = false;
		}

	queuedTasks.push_back(&task);
}

void Scheduler::close(){

	for (unsigned int i = 0; i < spinLocks.size(); i++){
		spinLocks[i]->unlock();
	}
	
	for (unsigned int i = 0; i < threads.size(); i++){
		threads[i]->join();
	}

	std::cout << "finished counter:" << counter << std::endl;
}

bool Scheduler::getIsConstructed(){
	return isConstructed;
}