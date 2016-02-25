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
		thread *t = new thread(&Fiber::runAndFree, fibers[i], startingTask);
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
		if (fibers[0]->isFiberFree())
		{
			//switching the fiber to a new fiber
			Fiber *temp = fibers[0];
			fibers[0] = new Fiber(counter,0);
			fibers[0]->run(task); //note not run and free because this is thread 0
			temp->switchOut();
			//fibers[0]->switchOut();

			//wait til unlock is completed
			//while (fibers[0]->isFiberFree() == false){}


			onceflag = false;
		}
	}
	//queuedTasks.push_back(&task);

	//end process after first task is completed
	//while(fibers[0]->isFiberFree() == false){}
	system("pause");
	close();
}

//end all the threads and notify main thread that the process has ended
void Scheduler::close(){
	//joins all the threads
	for (unsigned int i = 0; i < threads.size(); i++){
		threads[i]->join();
	}

	//tell the main thread that the process is ending
	endProcess.store(true, std::memory_order_relaxed);
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