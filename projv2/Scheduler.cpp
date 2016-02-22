#include "Scheduler.h"
#include <iostream>
#include <functional>
#include "Timer.h"

using namespace std::placeholders;




Scheduler::Scheduler(unsigned const int FIBER_COUNT, unsigned const int THREAD_COUNT){
	N_FIBER_PTR = &FIBER_COUNT;
	N_THREAD_PTR = &THREAD_COUNT;

	for (unsigned int i = 0; i < THREAD_COUNT; i++){
		spinLocks.push_back(new SpinLock());
	}

	
	std::function<void()> a = std::bind(&SpinLock::lock, spinLocks[0]);
	std::function<void()> b = std::bind(&SpinLock::unlock, spinLocks[0]);
	task = new Task(a);
	
	
	for (unsigned int i = 0; i < FIBER_COUNT; i++){
		fibers.push_back(new Fiber(counter));
		//fibers[i]->runAndFree(*task);
	}

	std::function<void(Task&)> assignFiber0 = std::bind(&Fiber::run,fibers[0], _1);

	//unsigned int threadCount = 2;
	//start all the threads in a spin lock
	for (unsigned int i = 0; i < THREAD_COUNT; i++){
		thread *t = new thread(&Fiber::run, fibers[0], *task);
		threads.push_back(t);
	}

	Timer *t = new Timer();
	t->wait(2);
	//std::cout << "here" << std::endl;
	spinLocks[0]->unlock();
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
	counter++;
	//try find an available fiber
	for (unsigned int i = 0; i<fibers.size(); i++)
		if (fibers[i]->isFiberFree()){
			fibers[i]->runAndFree(task);
			return;
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
	
}