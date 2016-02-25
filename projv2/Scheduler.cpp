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
	if (FIBER_COUNT < THREAD_COUNT){

		global::writeLock();
		std::cout << "Scheduler not started!" << std::endl <<
			"Need more fibers: fibers >= threads" << std::endl <<
			"Thread count: " << THREAD_COUNT << std::endl <<
			"Fiber count: " << FIBER_COUNT << std::endl;
		global::writeUnlock();

		isConstructed = false;
		return;
	}

	for (unsigned int i = 0; i < FIBER_COUNT; i++){
		fibers.push_back(new Fiber(counter, i));
	}

	for (unsigned int i = 0; i < THREAD_COUNT; i++){
		locks.push_back(new SpinLock());
		thread *t;
		if (i == 0)
			t = new thread(&Fiber::runAndFree, fibers[i], startingTask);
		else
			t = new thread(&SpinLock::lock, locks[i]);
		threads.push_back(t);
	}
	
	global::writeLock();
	std::cout << "Threads assgined to spin locks" << std::endl;
	global::writeUnlock();
	
	//wait until all spin locks have started
	//waitForThreadsFreed();
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
	global::writeLock();
	std::cout << "Assigning task" << std::endl;
	global::writeUnlock();

	Timer *t = new Timer();
	//try find an available fiber
	bool onceflag = true;
	while(onceflag){
		if (fibers[1]->isFiberFree())
		{
			//unlock standard lock oofr this thread
			if (locks[1]->getIsLocked())
				locks[1]->unlock();
			threads[1]->join();

			//set the next task on the fiber
			fibers[1]->setTask(task);
			
			//in future version, make a wrapper for fibers instead of creating new threads
			thread * temp = threads[1];
			threads[1] = new thread(&Fiber::runAndFree, fibers[1], task);
			delete temp;

			onceflag = false;
		}
	}
	//queuedTasks.push_back(&task);

	//end process after first task is completed
	//while(fibers[0]->isFiberFree() == false){}
	
	switchOutAllFibers();
	global::writeLock();
	system("pause");
	global::writeUnlock();
	close();
}

//end all the threads and notify main thread that the process has ended
void Scheduler::close(){
	//joins all the threads
	for (unsigned int i = 0; i < threads.size(); i++){

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
void Scheduler::switchOutAllFibers(){
	for (unsigned int i = 0; i < fibers.size(); i++){
		while (fibers[i]->isFiberFree() == false){}
		fibers[i]->switchOut();
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