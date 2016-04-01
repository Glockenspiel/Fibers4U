#include "include/Scheduler.h"
#include <iostream>
#include <functional>

using namespace std::placeholders;
using namespace fbr::priority;
using namespace scheduler_vars;

namespace fbr{
	
	Scheduler::Scheduler(const unsigned int FIBER_COUNT, const unsigned int THREAD_COUNT,
		BaseTask* startingTask, bool fiberAreDynamic, bool enableSleeping){
		useDynamicFibers = fiberAreDynamic;
		sleepingEnabled = enableSleeping;

		//do check on fiber and thread count, display error message if true
		if (FIBER_COUNT < THREAD_COUNT && FIBER_COUNT>0 && useDynamicFibers == false){
			fbr::con_cout_warn << "Scheduler not started!" << fbr::endl <<
				"Need more fibers" << fbr::endl <<
				"Thread count: " << THREAD_COUNT << fbr::endl <<
				"Fiber count: " << FIBER_COUNT << fbr::endl <<
				"Fiber count is less than thread count or else there's no fibers" << fbr::endl;

			system("pause");

			isConstructed = false;
			return;
		}

		//construct FiberPool and its fibers
		//if dynamic make sure there is at least 1 fiber per worker
		if (useDynamicFibers && FIBER_COUNT < THREAD_COUNT)
			fiberPool = new FiberPool(THREAD_COUNT);
		else
			fiberPool = new FiberPool(FIBER_COUNT);

		Counter constuctorCtr("Constructor");
		constuctorCtr += THREAD_COUNT;

		//create worker threads
		for (unsigned int i = 0; i < THREAD_COUNT; i++){
			workers.push_back(new Worker(i));

			fiberPool->fibers[i]->tryAcquire();
			thread *t;
			//assign starting task the the first worker thread
			if (i == 0){
				fiberPool->fibers[i]->setTask(startingTask, Priority::low, &constuctorCtr);
				workers[i]->set(*fiberPool->fibers[i]);
			}
			//assign empty tasks to the rest of the worker threads
			else{
				Task<> *emptyTask = new Task<>(&Scheduler::empty);
				fiberPool->fibers[i]->setTask(emptyTask, Priority::low, &constuctorCtr);
				workers[i]->set(*fiberPool->fibers[i]);
			}

			t = new thread(&Worker::run, workers[i]);
			fiberPool->fibers[i]->setPrepared();
			threads.push_back(t);
		}

		while (constuctorCtr.get() > 0);
	}

	//destructor
	Scheduler::~Scheduler(){

		delete fiberPool;

		//delete all the threads
		for (unsigned int i = 0; i < threads.size(); i++)
			delete threads[i];
		threads.clear();

		for (unsigned int i = 0; i < workers.size(); i++)
			delete workers[i];
		workers.clear();
	}

	//adds a taks to the queue with the given priority
	void Scheduler::addToQueue(BaseTask *task, Priority taskPrioirty, Counter* ctr){
		Fiber* fiber;

		//aquire free fiber
		do{
			fiber = fiberPool->tryAcquireFreeFiber();

			//no free fiber found
			if (fiber == nullptr){
				//if dynamic create a new fiber
				if (useDynamicFibers){
					while (dynamicFiberLock.test_and_set(std::memory_order_seq_cst));
					fiberPool->fibers.push_back(new Fiber(fiberPool->fibers.size()));
					dynamicFiberLock.clear(std::memory_order_seq_cst);
				}
				//display warning of the unessasary spin lock
				else{
					fbr::con_cout_warn << "Task waiting for free fiber." << fbr::endl <<
						"Create more fibers to prevent unessasary waiting." << fbr::endl <<
						"This can also cause dealock." << fbr::endl;
				}
			}
		} while (fiber == nullptr);

		fiber->setTask(task, taskPrioirty, ctr);
		fiberPool->pushToQueue(*fiber);
	}

	//adds the tasks to the queue and then run them
	//this also clears the vector passed in
	//the tasks will be cleaned up by the scheduler
	void Scheduler::runTasks(vector<BaseTask*> tasks, Priority taskPriority, Counter* ctr){
		ctr->add(tasks.size());

		for (BaseTask* task : tasks)
			addToQueue(task, taskPriority, ctr);

		//only notify the free workers
		notifyFreeWorkers(tasks.size());

		//tidy up vector
		tasks.clear();
	}

	//variadic function overload of runTasks()
	void Scheduler::runTasks(Priority prio, Counter* ctr, unsigned int count, BaseTask*...){
		ctr->add(count);
		int curCount = count;
		va_list args;
		va_start(args, count);

		while (curCount > 0){
			BaseTask * bt = va_arg(args, BaseTask*);
			addToQueue(bt, prio, ctr);
			curCount--;
		}
		va_end(args);

		notifyFreeWorkers(count);
	}

	//tells a certain amount of workers that the queue has been populated again
	void Scheduler::notifyFreeWorkers(int max){
		for (unsigned int i = 0; max > 0 && i < workers.size(); i++){
			if (workers[i]->getState() == Worker::free){
				notifyWorkerBeenFreed(workers[i]);
				max--;
			}
		}
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
		for (unsigned int i = 0; i < counters.size(); i++)
			while (counters[i]->get() > 0);
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
		mainThreadSleeper.wakeUp();
	}

	//this allows for the a thread to yield until wakeUpMain() is called
	//only use this to yield the main thread
	void Scheduler::waitMain(){
		mainThreadSleeper.sleep();
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
		/*
		//queue is empty
		else{
			fbr::con_cout << "queue is empty" << fbr::endl;
		}
		*/
	}

	//allows a task to wait until counter reaches a point without spinlocking a worker thread
	//by adding this task to a waiting queue and checking it each time a worker thread is freed.
	//if the counter is less than or equal to the waiting tasks waiting count, the task will just be run.
	void Scheduler::waitForCounter(WaitingTask& task){
		//if all workers are free, just run rather than waiting
		if (task.isReadyToRun()){
			runTasks({ task.getTask() }, task.getPriority(), task.getTaskCounter());
		}
		//send to waiting queue
		else{
			//increment the counter 
			task.getTaskCounter()->add(1);

			//acquire lock for accessing waitingTasks
			while (waitingLock.test_and_set(std::memory_order_seq_cst));

			waitingTasks.push_back(&task);

			//release waitingTasks
			waitingLock.clear(std::memory_order_seq_cst);
		}
	}

	//abbreviated version of waitForCounter
	void Scheduler::waitForCounter(Counter* ctr, unsigned int count, BaseTask* task, Counter* taskCtr){
		WaitingTask* wt = new WaitingTask(ctr, task, count, Priority::high, taskCtr);
		waitForCounter(*wt);
	}

	//checks if there is any waiting tasks to be added to the queue
	void Scheduler::checkWaitingTasks(){

		//acquire lock for accessing waitingTasks
		while (waitingLock.test_and_set(std::memory_order_release));

		for (unsigned int i = 0; i < waitingTasks.size(); i++){
			//if waiting count >= counter, run the task with high priority
			if (waitingTasks[i]->isReadyToRun()){

				addToQueue(waitingTasks.at(i)->getTask(), waitingTasks.at(i)->getPriority(), waitingTasks.at(i)->getTaskCounter());

				//remove waiting task from vector
				delete waitingTasks[i];
				waitingTasks.erase(i);
			}
		}

		//release waitingTasks
		waitingLock.clear(std::memory_order_relaxed);
	}

	bool Scheduler::isSleepingEnabled(){
		return sleepingEnabled.get();
	}

	void Scheduler::addCounter(Counter& counter){
		counters.push_back(&counter);
	}

	int Scheduler::getCounterValByName(std::string name){
		int count=-1;
		bool found = false;
		counters.getLock();
		for (unsigned int i = 0; i < counters.size_async() && !found; i++){
			
			if (counters.at_async(i)->getName() == name)
			{
				count = counters.at_async(i)->get();
				found = true;
			}
		}
		counters.unlock();

		return count;
	}
}