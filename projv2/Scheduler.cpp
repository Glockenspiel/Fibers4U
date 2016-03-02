#include "Scheduler.h"
#include <iostream>
#include <functional>
#include "Timer.h"
#include "Player.h"
#include "Task.h"
//#include "Global.h"

using namespace std::placeholders;
using namespace priority;
using namespace count;

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
	fiberPool = new FiberPool(FIBER_COUNT);

	counter = THREAD_COUNT;
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
			Task *emptyTask = new Task(&Scheduler::empty);
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
	//fbr::cout << "runTask() started" << fbr::endl;
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
	//while (queueLock.test_and_set(std::memory_order_seq_cst));

	//try find a free worker
	//wkr = acquireFreeWorker();

	//add the task queue
	//if (wkr == nullptr || fiberPool->queueHasNext()){

		fbr->setTask(task, taskPrioirty);
		fiberPool->pushToQueue(*fbr);
		/*
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
	*/
	//release access to queue
	//queueLock.clear(std::memory_order_seq_cst);
	//fbr::cout << "After runTask() counter:" << counter.load(std::memory_order_seq_cst) << fbr::endl;
}

//run multiple tasks with low priority
void Scheduler::runTasks(vector<BaseTask*> tasks){
	runTasks(tasks, priority::low);
}

//run multiple tasks with a given priority
void Scheduler::runTasks(vector<BaseTask*> tasks, Priority taskPriority){
	while (queueLock.test_and_set(std::memory_order_seq_cst));
	for (BaseTask* task : tasks)
		runTask(task, taskPriority);
	
	queueLock.clear(std::memory_order_seq_cst);
	waitAllFibersFree();
	counter.fetch_add(tasks.size(), std::memory_order_seq_cst);
	fbr::cout << "starting task counter:" << counter << ":" << fiberPool->queueCount() << fbr::endl;
	for (Worker* w : workers){
		notifyWorkerBeenFreed(w);
	}
}

void Scheduler::notifyFinishedTask(){
	counter.fetch_sub(1, std::memory_order_seq_cst);
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
	//while (count::counter.load(std::memory_order_relaxed) != 0){}
	while (fiberPool->queueCount()>0){}
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
	fbr::cout << "WAKEWAKEWAKEWAKE" << fbr::endl;
	std::lock_guard<std::mutex> lk(*mainMtx);
	mainAwake = true;

	//wake up main thread
	mainCV.notify_one();
}

//this allows for the a thread to yield until wakeUpMain() is called
//only use this to yield the main thread
void Scheduler::waitMain(){
	std::unique_lock<std::mutex> lk(*mainMtx);
	mainCV.wait(lk, []{return mainAwake; });
	lk.unlock();
}

//this is called by a worker when the worker had been freed.
//this function will see if there is any fibers in the queue and 
//run the next fiber
void Scheduler::notifyWorkerBeenFreed(Worker* worker){
	
	fbr::cout << "Task completed: " << count::counter.load(std::memory_order_relaxed) << ":"<< fiberPool->queueCount() << fbr::endl;
	
	//fbr::cout << "checkWaitingTasks() started" << fbr::endl;
	//checkWaitingTasks();
	//fbr::cout << "checkWaitingTasks() ended" << fbr::endl;

	//access the queue
	while (queueLock.test_and_set(std::memory_order_seq_cst));

	//check if there is any queued fibers
	if (fiberPool->queueHasNext()){
		//acquire worker
		
		fbr::cout << "queue has next" << fbr::endl;
		worker->forceAcquire();
		//fbr::cout << "force acquire ended" << fbr::endl;
		//get next fiber in the queue using FIFO(first in first out)
		Fiber* nextFiber;
		nextFiber = &fiberPool->popNextFiber();

		queueLock.clear(std::memory_order_seq_cst);

		//run fiber on the worker
		worker->set(*nextFiber);
		nextFiber->setPrepared();
	}
	else{
		//release the queue
		
		fbr::cout << "queue is empty" << fbr::endl;
		queueLock.clear(std::memory_order_seq_cst);
	}

	fbr::cout << "notify completed" << fbr::endl;
}

//abbreviated version of waitForCounter
void Scheduler::waitForCounter(int count, BaseTask* task){
	WaitingTask* wt = new WaitingTask(task, count, Priority::high);
	waitForCounter(*wt);
}

//allows a task to wait until counter reaches a point without spinlocking a worker thread
void Scheduler::waitForCounter(WaitingTask& task){
	//if wait is not required, just run the task
	if (task.getWaitingCount() >= count::counter.load(std::memory_order_relaxed)){
		counter += 1;
		runTask(task.getTask(), task.getPriority());
	}

	//if waiting is required add this task to the waitingTasks list
	else{
		//acquire lock for accessing waitingTasks
		while (waitingLock.test_and_set(std::memory_order_seq_cst));

		waitingTasks.push_back(&task);

		//release waitingTasks
		waitingLock.clear(std::memory_order_seq_cst);

	}
}

void Scheduler::checkWaitingTasks(){
	//acquire lock for accessing waitingTasks
	
	while (waitingLock.test_and_set(std::memory_order_release));

	for (unsigned int i = 0; i < waitingTasks.size(); i++){
		//if waiting count >= counter, run the task with high priority
		if (waitingTasks[i]->getWaitingCount() >= counter.load(std::memory_order_acquire)){
			counter += 1;
			runTask(waitingTasks[i]->getTask(), Priority::high);

			//remove waiting task from vector
			delete waitingTasks[i];
			waitingTasks.erase(waitingTasks.begin()+i);
			i--;
		}
	}


	//release waitingTasks
	waitingLock.clear(std::memory_order_relaxed);

	
}