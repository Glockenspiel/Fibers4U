#include "Scheduler.h"
#include <iostream>
#include <functional>
#include "Timer.h"
#include "Player.h"
#include "Task.h"
//#include "Global.h"

using namespace std::placeholders;
using namespace priority;

//varaible used in lambda expression to notify main thread to wake up
bool mainAwake = false;

Scheduler::Scheduler(unsigned const int FIBER_COUNT, unsigned const int THREAD_COUNT, 
	BaseTask* startingTask){
	N_FIBER_PTR = &FIBER_COUNT;
	N_THREAD_PTR = &THREAD_COUNT;
	mainMtx = new mutex();

	//do check on fiber and thread count, display error message if true
	if (FIBER_COUNT < THREAD_COUNT && FIBER_COUNT>0){
		fbr::cout_warn << "Scheduler not started!" << fbr::endl <<
			"Need more fibers" << fbr::endl <<
			"Thread count: " << THREAD_COUNT << fbr::endl <<
			"Fiber count: " << FIBER_COUNT << fbr::endl <<
			"Fiber count is less than thread count or else there's no fibers" << fbr::endl;

		system("pause");

		isConstructed = false;
		return;
	}

	//construct FiberPool and its fibers
	fiberPool = new FiberPool(FIBER_COUNT, counter);

	//create worker threads
	for (unsigned int i = 0; i < THREAD_COUNT; i++){
		workers.push_back(new Worker(i));
		
		fiberPool->fibers[i]->tryAcquire();
		thread *t;
		//assign starting task the the first worker thread
		if (i == 0){
			fiberPool->fibers[i]->setTask(startingTask, Priority::low);
			workers[i]->set(*fiberPool->fibers[i]);
		}
		//assign empty tasks to the rest of the worker threads
		else{
			BaseTask *emptyTask = new Task(&Scheduler::empty, this);
			fiberPool->fibers[i]->setTask(emptyTask, Priority::low);
			workers[i]->set(*fiberPool->fibers[i]);
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

	//delete mainMtx;
}


//run the task with low priority
void Scheduler::runTask(BaseTask *task){
	runTask(task, priority::low);
}

//run the task with a given priority
void Scheduler::runTask(BaseTask *task, Priority taskPrioirty){
	//find available worker
	Fiber* fbr;
	Worker* wkr;
	BaseTask* nextTask;

	//aquire free fiber
	do{
		fbr = fiberPool->tryAcquireFreeFiber();

		//display warinign if spining for a free fiber
		if (fbr == nullptr){
			fbr::cout_warn << "Task waiting for free fiber." << fbr::endl <<
				"Create more fibers to prevent unessasary waiting." << fbr::endl <<
				"This can also cause dealock." << fbr::endl;
		}
	} while (fbr==nullptr);


	//lock when accessing queue
	while (queueLock.test_and_set(std::memory_order_acquire));

	//try find a free worker
	wkr = acquireFreeWorker();

	//add the task queue
	if (wkr == nullptr || fiberPool->queueHasNext()){

		fbr->setTask(task, taskPrioirty);
		fiberPool->pushToQueue(*fbr);

	}
	//no queuing needed
	else{
		nextTask = task;

		//set values then run, must be in prepared state to change to run state
		fbr->setTask(task, taskPrioirty);
		wkr->set(*fbr);

		//finished setting values so now prepare for running
		fbr->setPrepared();
	}
	
	//release access to queue
	queueLock.clear(std::memory_order_release);
}

//run multiple tasks with low priority
void Scheduler::runTasks(vector<BaseTask*> tasks){
	for (BaseTask* task : tasks)
		runTask(task);
}

//run multiple tasks with a given priority
void Scheduler::runTasks(vector<BaseTask*> tasks, Priority taskPriority){
	for (BaseTask* task : tasks)
		runTask(task, taskPriority);
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
	std::lock_guard<std::mutex> lk(*mainMtx);
	mainAwake = true;

	//wake up main thread
	mainCV.notify_one();
}

void Scheduler::waitMain(){
	std::unique_lock<std::mutex> lk(*mainMtx);
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
	if (fiberPool->queueHasNext()){
		//acquire worker
		worker->forceAcquire();

		//get next fiber in the queue using FIFO(first in first out)
		Fiber* nextFiber;
		nextFiber = &fiberPool->popNextFiber();

		//run fiber on the worker
		worker->set(*nextFiber);
		nextFiber->setPrepared();
	}

	//release the queue
	queueLock.clear(std::memory_order_release);
}