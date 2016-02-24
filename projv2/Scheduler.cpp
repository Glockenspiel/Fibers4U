#include "Scheduler.h"
#include <iostream>
#include <functional>
#include "Timer.h"
//#include "Global.h"

using namespace std::placeholders;




Scheduler::Scheduler(unsigned const int FIBER_COUNT, unsigned const int THREAD_COUNT){
	N_FIBER_PTR = &FIBER_COUNT;
	N_THREAD_PTR = &THREAD_COUNT;

	if (FIBER_COUNT < THREAD_COUNT){
		//global::writeLock();

		std::cout << "Scheduler not started!" << std::endl <<
			"Need more fibers: fibers >= threads" << std::endl <<
			"Thread count: " << THREAD_COUNT << std::endl <<
			"Fiber count: " << FIBER_COUNT << std::endl;

		//global::writeUnlock();
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

		//create the task and push it onto the fiber
		tasks.push_back(new Task(fn));
		thread *t = new thread(&Fiber::runAndFree, fibers[i], *tasks[i]);
		threads.push_back(t);
	}
	
	global::writeLock();
	std::cout << "Threads assgined to spin locks" << std::endl;
	global::writeUnlock();
	
	//waitForThreadsFreed();

	//spinLocks[0]->unlock();
	/*
	//wait till all spin lock starts
	for(unsigned int index=0; index<THREAD_COUNT; index++){
		if (spinLocks[index]->getIsLocked()){
			spinLocks[index]->unlock();
		}
	}
	*/
	
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
	global::writeLock();
	std::cout << "Assigning task" << std::endl;
	global::writeUnlock();

	//try find an available fiber
	bool flag = true;
	for (unsigned int i = 0; i<fibers.size() && flag; i++)
		if (spinLocks[i]->getIsLocked()){ //must change this so it doesnt have to retruns to a spin lock to switch to a new task

			spinLocks[i]->unlock();
			//wait til unlock is completed
			while (spinLocks[i]->getIsLocked()){}

			//run new task
			fibers[i]->runAndFree(task);
			flag = false;
		}

	queuedTasks.push_back(&task);
	//waitForThreadsFreed();

	Timer *t = new Timer();
	t->wait(2);
	isCompleted = true;
	close();
	
	system("pause");
}

void Scheduler::close(){
	//unlocks all spin locks
	for (unsigned int i = 0; i < spinLocks.size(); i++){
		spinLocks[i]->unlock();
	}
	
	//joins all the threads
	for (unsigned int i = 0; i < threads.size(); i++){
		threads[i]->join();
	}

	//isCompleted.store(true, std::memory_order_aquire);
}



bool Scheduler::getIsConstructed(){
	return isConstructed;
}

//wait for all threads to reach spin locks, then return
void Scheduler::waitForThreadsFreed(){
	for (unsigned int index = 0; index<spinLocks.size(); index++){
		while (spinLocks[index]->getIsLocked() == false){} 
	}
}