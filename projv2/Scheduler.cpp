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

	taskCounter+=THREAD_COUNT;

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
		//fiberPool->workerStarted();
		t = new thread(&Worker::run, workers[i]);
		fiberPool->fibers[i]->setPrepared();
		threads.push_back(t);
	}
	waitAllFibersFree();
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
}

//adds a taks to the queue with the given priority
void Scheduler::addToQueue(BaseTask *task, Priority taskPrioirty){
	Fiber* fiber;

	//aquire free fiber
	do{
		fiber = fiberPool->tryAcquireFreeFiber();

		//display warining if spining for a free fiber
		if (fiber == nullptr){
			fbr::cout_warn << "Task waiting for free fiber." << fbr::endl <<
				"Create more fibers to prevent unessasary waiting." << fbr::endl <<
				"This can also cause dealock." << fbr::endl;
		}
	} while (fiber==nullptr);

	fiber->setTask(task, taskPrioirty);
	fiberPool->pushToQueue(*fiber);
	taskCounter++;
}

//adds the tasks to the queue and then run them
//this also clears the vector passed in
//the tasks will be cleaned up by the scheduler
void Scheduler::runTasks(vector<BaseTask*> tasks, Priority taskPriority){
	for (BaseTask* task : tasks)
		addToQueue(task, taskPriority);
	
	//only notify the free workers
	for (unsigned int i = 0; i < tasks.size() && i < workers.size(); i++){
		if (workers[i]->getState() == Worker::free);
			notifyWorkerBeenFreed(workers[i]);
	}

	//tidy up vector
	tasks.clear();
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

//waits in a spinlock until all current tasks are completed i.e. when the counter reaches zero
//note: this will consume the current thread which calls this function
void Scheduler::waitAllFibersFree(){
	while (taskCounter.get()>0){}
}


//returns true if schduler constructed correctly
bool Scheduler::getIsConstructed(){
	return isConstructed;
}


//trys to acequire a free worker, returns nullptr if it fails
Worker* Scheduler::tryAcquireFreeWorker(){
	for (unsigned int i = 0; i < workers.size(); i++){
		if (workers[i]->tryAcquire())
			return workers[i];
	}

	return nullptr;
}

//empty function for initialising worker threads
void Scheduler::empty(){}

//wakes up the main thread to end the programs execution
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
//this function will first check if any waiting tasks should be added to the queue
//then see if there is any fibers in the queue and run the next fiber in the queue
void Scheduler::notifyWorkerBeenFreed(Worker* worker){
	//check if any waiting tasks should be added to the queue
	checkWaitingTasks();

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
	else{
		fbr::cout << "queue is empty" << fbr::endl;
	}
	fbr::cout << "Task completed: " << taskCounter.get() << fbr::endl;
}

//allows a task to wait until counter reaches a point without spinlocking a worker thread
//by adding this task to a waiting queue and checking it each time a worker thread is freed.
//if the counter is less than or equal to the waiting tasks waiting count, the task will just be run.
void Scheduler::waitForCounter(WaitingTask& task){
	//if all workers are free, just run rather than waiting
	if (taskCounter.get()<=task.getWaitingCount()){
		runTasks({ task.getTask() }, task.getPriority());
	}
	//send to waiting queue
	else{
		//acquire lock for accessing waitingTasks
		while (waitingLock.test_and_set(std::memory_order_seq_cst));

		waitingTasks.push_back(&task);

		//release waitingTasks
		waitingLock.clear(std::memory_order_seq_cst);
	}
}

//abbreviated version of waitForCounter
void Scheduler::waitForCounter(unsigned int count, BaseTask* task){
	WaitingTask* wt = new WaitingTask(task, count, Priority::high);
	waitForCounter(*wt);
}

//checks if there is any waiting tasks to be added to the queue
void Scheduler::checkWaitingTasks(){

	//acquire lock for accessing waitingTasks
	while (waitingLock.test_and_set(std::memory_order_release));

	for (unsigned int i = 0; i < waitingTasks.size(); i++){
		//if waiting count >= counter, run the task with high priority
		if (waitingTasks[i]->getWaitingCount() >= taskCounter.get()){

			addToQueue(waitingTasks.at(i)->getTask(), Priority::high);

			//remove waiting task from vector
			delete waitingTasks[i];
			waitingTasks.erase(i);
		}
	}

	//release waitingTasks
	waitingLock.clear(std::memory_order_relaxed);
}

//worker threads can notify the scheduler that a task has finished
void Scheduler::notifyTaskFinished(){
	taskCounter--;
}