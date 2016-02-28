#include "Scheduler.h"
#include <iostream>
#include <functional>
#include "Timer.h"
#include "Player.h"
//#include "Global.h"

using namespace std::placeholders;


//varaible used in lambda expression to notify main thread to wake up
bool mainAwake = false;
static vector<Fiber*> queuedFibers;
static mutex *queueMutex;


Scheduler::Scheduler(unsigned const int FIBER_COUNT, unsigned const int THREAD_COUNT, 
	Task& startingTask){
	N_FIBER_PTR = &FIBER_COUNT;
	N_THREAD_PTR = &THREAD_COUNT;
	mtx = new mutex();
	queueMutex = new mutex();

	if (FIBER_COUNT < THREAD_COUNT && FIBER_COUNT>0){

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
	}

	for (unsigned int i = 0; i < THREAD_COUNT; i++){
		workers.push_back(new Worker(i));
		
		fibers[i]->tryAcquire();
		thread *t;
		if (i == 0){
			workers[i]->set(startingTask, *fibers[i]);
		}
		else{
			function<void()> emptyFunc = std::bind(&Scheduler::empty, this);
			Task *emptyTask = new Task(emptyFunc);
			workers[i]->set(*emptyTask, *fibers[i]);
		}
		t = new thread(&Worker::run, workers[i]);
		fibers[i]->setPrepared();
		threads.push_back(t);
	}
}

//destructor
Scheduler::~Scheduler(){
	//delete all fibers
	for (Fiber* f : fibers)
		delete f;

	//delete all the threads
	for (thread* t : threads)
		delete t;
}


//run the task given on a fiber
void Scheduler::runTask(Task &task){
	//find available worker
	Fiber* fbr;
	Worker* wkr;
	Task* nextTask;

	//aquire free fiber
	do{
		fbr = acquireFreeFiber();

		//display warinign if spining for a free fiber
		if (fbr == nullptr){
			global::writeLock();
			std::cout << "Warning!: Task waiting for free fiber" << std::endl <<
				"Create more fibers to prevent unessasary waiting" << std::endl;
			global::writeUnlock();
		}
	} while (fbr==nullptr);


	//lock when accessing queue
	queueMutex->lock();

	//try find a free worker
	wkr = acquireFreeWorker();

	//add the task queue
	if (wkr == nullptr || queuedFibers.size()>0){

		fbr->setTask(task);
		queuedFibers.push_back(fbr);
	}
	//no queuing needed
	else{
		nextTask = &task;

		//set values then run, must be in prepared state to change to run state
		wkr->set(*nextTask, *fbr);

		//finished setting values so now prepare for running
		fbr->setPrepared();
	}

	//release access to queue
	queueMutex->unlock();
}

//end all the threads and notify main thread that the process has ended
void Scheduler::close(){
	waitAllFibersFree();

	//joins all the threads
	for (unsigned int i = 0; i < threads.size(); i++){
		
		workers[i]->close();

		if (threads[i]->joinable()){
			threads[i]->join();
		}
	}
}

//waits until all current tasks are completed i.e. when the counter reaches zero
void Scheduler::waitAllFibersFree(){
	while (counter.load(std::memory_order_relaxed) != 0){}
}


//returns true if schduler constructed correctly
bool Scheduler::getIsConstructed(){
	return isConstructed;
}

Fiber* Scheduler::acquireFreeFiber(){
	for (unsigned int i = 0; i < fibers.size(); i++){
			if(fibers[i]->tryAcquire()) //try acquire
				return fibers[i];
	}
	return nullptr;
}

//trys to acequire a free worker
Worker* Scheduler::acquireFreeWorker(){
	for (unsigned int i = 0; i < workers.size(); i++){
		if (workers[i]->tryAcquire())
			return workers[i];
	}

	return nullptr;
}

//empty function for initialising worker threads
void Scheduler::empty(){}

void Scheduler::wakeUpMain(){
	std::lock_guard<std::mutex> lk(*mtx);
	mainAwake = true;

	//wake up main thread
	mainCV.notify_one();
}

void Scheduler::waitMain(){
	std::unique_lock<std::mutex> lk(*mtx);
	mainCV.wait(lk, []{return mainAwake; });
	lk.unlock();
}

//this is called by a worker when the worker had been freed.
//this function will see if there is any fibers in the queue and 
//run the next fiber
void Scheduler::workerBeenFreed(Worker* worker){

	//access the queue
	queueMutex->lock();

	//check if there is any queued fibers
	if (queuedFibers.size() > 0){
		//acquire worker
		worker->forceAcquire();

		//get next fiber with FIFO(first in first out)
		Fiber* nextFiber = queuedFibers.front();
		queuedFibers.erase(queuedFibers.begin());

		//run fiber on the worker
		worker->set(*nextFiber);
		nextFiber->setPrepared();
	}

	//release the queue
	queueMutex->unlock();
}