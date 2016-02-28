#include "Scheduler.h"
#include <iostream>
#include <functional>
#include "Timer.h"
#include "Player.h"
#include "Task.h"
//#include "Global.h"

using namespace std::placeholders;

//varaible used in lambda expression to notify main thread to wake up
bool mainAwake = false;

Scheduler::Scheduler(unsigned const int FIBER_COUNT, unsigned const int THREAD_COUNT, 
	BaseTask* startingTask){
	N_FIBER_PTR = &FIBER_COUNT;
	N_THREAD_PTR = &THREAD_COUNT;
	mtx = new mutex();

	//do check on fiber and thread count, display error message if true
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

	//create fibers
	//for (unsigned int i = 0; i < FIBER_COUNT; i++){
	//	fibers.push_back(new Fiber(counter, i));
	//}
	fiberPool = new FiberPool(FIBER_COUNT, counter);

	//create worker threads
	for (unsigned int i = 0; i < THREAD_COUNT; i++){
		workers.push_back(new Worker(i));
		
		fiberPool->fibers[i]->tryAcquire();
		thread *t;
		//assign starting task the the first worker thread
		if (i == 0){
			workers[i]->set(startingTask, *fiberPool->fibers[i]);
		}
		//assign empty tasks to the rest of the worker threads
		else{
			BaseTask *emptyTask = new Task(&Scheduler::empty, this);
			workers[i]->set(emptyTask, *fiberPool->fibers[i]);
		}
		t = new thread(&Worker::run, workers[i]);
		fiberPool->fibers[i]->setPrepared();
		threads.push_back(t);
	}
}

//destructor
Scheduler::~Scheduler(){
	delete fiberPool;

	//delete all the threads
	for (thread* t : threads)
		delete t;
	threads.clear();

	for (Worker* w : workers)
		delete w;
	workers.clear();

	delete mtx;
}


//run the task given on a fiber
void Scheduler::runTask(BaseTask *task){
	//find available worker
	Fiber* fbr;
	Worker* wkr;
	BaseTask* nextTask;

	//aquire free fiber
	do{
		fbr = fiberPool->acquireFreeFiber();

		//display warinign if spining for a free fiber
		if (fbr == nullptr){
			global::writeLock();
			std::cout << "Warning!: Task waiting for free fiber" << std::endl <<
				"Create more fibers to prevent unessasary waiting" << std::endl;
			global::writeUnlock();
		}
	} while (fbr==nullptr);


	//lock when accessing queue
	while (queueLock.test_and_set(std::memory_order_acquire));

	//try find a free worker
	wkr = acquireFreeWorker();

	//add the task queue
	if (wkr == nullptr || fiberQueue.size()>0){

		fbr->setTask(task);
		fiberQueue.push(fbr);
	}
	//no queuing needed
	else{
		nextTask = task;

		//set values then run, must be in prepared state to change to run state
		wkr->set(nextTask, *fbr);

		//finished setting values so now prepare for running
		fbr->setPrepared();
	}
	
	//release access to queue
	queueLock.clear(std::memory_order_release);
}

void Scheduler::runTasks(vector<BaseTask*> tasks){
	for (BaseTask* task : tasks)
		runTask(task);
}

//end all the threads and notify main thread
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
	while (queueLock.test_and_set(std::memory_order_acquire));

	//check if there is any queued fibers
	if (fiberQueue.empty() == false){
		//acquire worker
		worker->forceAcquire();

		//get next fiber in the queue using FIFO(first in first out)
		Fiber* nextFiber;
		nextFiber= fiberQueue.front();
		fiberQueue.pop();

		//run fiber on the worker
		worker->set(*nextFiber);
		nextFiber->setPrepared();
	}

	//release the queue
	queueLock.clear(std::memory_order_release);
}