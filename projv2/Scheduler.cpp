#include "Scheduler.h"
#include <iostream>
#include <functional>
#include "Timer.h"
//#include "Global.h"

using namespace std::placeholders;


Scheduler::Scheduler(unsigned const int FIBER_COUNT, unsigned const int THREAD_COUNT, Task& startingTask){
	N_FIBER_PTR = &FIBER_COUNT;
	N_THREAD_PTR = &THREAD_COUNT;

	//display error msg

	if (FIBER_COUNT < THREAD_COUNT && FIBER_COUNT>0){

	#pragma message ("Sceduler won't start, you must set more fibers");

		global::writeLock();
		std::cout << "Scheduler not started!" << std::endl <<
			"Need more fibers" << std::endl <<
			"Thread count: " << THREAD_COUNT << std::endl <<
			"Fiber count: " << FIBER_COUNT << std::endl <<
			"Fiber count is less than thread count or else there's no fibers" << std::endl;
		global::writeUnlock();

		system("pause");

		isConstructed = false;
		return;
	}

	for (unsigned int i = 0; i < FIBER_COUNT; i++){
		fibers.push_back(new Fiber(counter, i));
		fibers[i]->tryAcquire();
	}

	for (unsigned int i = 0; i < THREAD_COUNT; i++){
		locks.push_back(new SpinLock());
		workers.push_back(new Worker());
		
		thread *t;
		if (i == 0){
			workers[i]->set(startingTask, *fibers[i]);
			//t = new thread(&Fiber::runAndFree, fibers[i], startingTask);
		}
		else{
			function<void()> emptyFunc = std::bind(&Scheduler::workerThreadStart, this);
			Task *emptyTask = new Task(emptyFunc);
			workers[i]->set(*emptyTask, *fibers[i]);			
			//t = new thread(&SpinLock::lock, locks[i]);
		}
		t = new thread(&Worker::run, workers[i]);
		fibers[i]->setPrepared();
		threads.push_back(t);
	}
}

//destructor
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


//run the task given on a fiber
void Scheduler::runTask(Task &task){
	//find available fiber
	int index;
	//instead of loop use the taskQueue
	do{
		index = acquireFreeFiber();
	} while (index < 0);

	if (index < 0){
		return;
	}

	//set values then run, must be in prepared state to change to run state
	workers[index]->set(task, *fibers[index]);
	
	//finished setting values so now prepare for running
	fibers[index]->setPrepared();
}

//end all the threads and notify main thread that the process has ended
void Scheduler::close(){
	waitAllFibersFree();

	//joins all the threads
	for (unsigned int i = 0; i < threads.size(); i++){
		
		workers[i]->close();

		if (locks[i]->getIsLocked())
			locks[i]->unlock();

		if (threads[i]->joinable()){
			threads[i]->join();
		}
	}

	//tell the main thread that the process is ending
	endProcess.store(true, std::memory_order_relaxed);
}

//waits until all current tasks are completed and then ends all the spin locks in the fibers
void Scheduler::waitAllFibersFree(){
	for (unsigned int i = 0; i < fibers.size(); i++){
		fibers[i]->waitUntilFree();
	}
}


//returns true if schduler constructed correctly
bool Scheduler::getIsConstructed(){
	return isConstructed;
}

//wait for all threads to reach spin locks, then return
void Scheduler::waitForThreadsFreed(){
	//for (unsigned int index = 0; index<spinLocks.size(); index++){
	//	while (spinLocks[index]->getIsLocked() == false){} 
	//}
}

//returns the endProcess value
bool Scheduler::getEndProcess(){
	return endProcess.load(std::memory_order_relaxed);
}

void Scheduler::workerThreadStart(){
	//do nothing
}

int Scheduler::acquireFreeFiber(){
	for (unsigned int i = 0; i < fibers.size(); i++){
			if(fibers[i]->tryAcquire()) //try acquire
				return i;
	}
	return -1;
}