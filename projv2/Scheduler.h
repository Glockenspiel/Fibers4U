#ifndef SCHEDULER_H
#define SCHEDULER_H

#include "BaseTask.h"
#include "con_vector.h"
#include <atomic>
#include "Fiber.h"
#include <thread>
#include "con_iostream.h"
#include "Worker.h"
#include <mutex>
#include <condition_variable>
#include "FiberPool.h"
#include "WaitingTask.h"
#include "Counter.h"
#include <stdarg.h>
#include "concurrent.h"


using std::thread;
using std::mutex;

static atomic<bool> isCompleted = false;

//boolean to detect if the scheduler should use dynamic fibers
static bool useDynamicFibers;
static FiberPool *fiberPool;
static mutex *mainMtx;
static std::condition_variable mainCV;

static con_vector<WaitingTask *> waitingTasks;
static con_vector<Worker *> workers;


//atomic flag for accessing waitingTasks
static std::atomic_flag waitingLock = ATOMIC_FLAG_INIT;

//lock for adding dynamic fibers
static std::atomic_flag dynamicFiberLock = ATOMIC_FLAG_INIT;

static concurrent<bool> isSleepingEnabled = true;

using std::vector;

class Scheduler{
public:
	//intialises all the worker threads
	Scheduler(const unsigned int FIBER_COUNT, const unsigned int THREAD_COUNT,
		BaseTask* startingTask, bool fibersAreDynamic, bool enableSleeping);

	//destructor, must call close() first
	~Scheduler();
	
	//puts all the tasks into a queue and runs them
	static void runTasks(vector<BaseTask*> tasks, Priority taskPriority);

	//run tasks variadic function overload
	static void runTasks(Priority prio, unsigned int count, BaseTask*...);

	//waits for all queued fibers to complete and then joins the threads 
	//(must be called before deconstructor)
	void close();

	//returns true if construction was successful
	bool getIsConstructed();

	//spinlocks until all fibers have completed their tasks
	static void waitAllFibersFree();

	//put the task into a waiting queue until the task counter reaches thw waiting count
	static void waitForCounter(WaitingTask& task);

	//constructs a WaitingTask object and calls waitForCounter
	static void waitForCounter(unsigned int count, BaseTask* task);

	//checks the waiting queue to see if any of the tasks should be woken up
	static void checkWaitingTasks();

	//wakes up the main thread
	static void wakeUpMain();

	//call this in the main thread to make the thread yield until wakeUpMain() is called
	static void waitMain();

	//allows workers notify that they have been freed, 
	//then check the waiting queue and 
	//then run the next task in the queue if any
	static void notifyWorkerBeenFreed(Worker* worker);

	//notifies the task counter that a task has completed
	static void notifyTaskFinished();

	static bool sleepingEnabled();
	
private:
	//adds a task to the task queue
	static void addToQueue(BaseTask *task, priority::Priority taskPrioirty);

	//tries to acquire a free worker, return nullptr if failed
	static Worker* tryAcquireFreeWorker();

	//tells a certain amount of workers that the queue has been populated again
	static void notifyFreeWorkers(int max);

	//empty function used for creating worker threads
	static void empty();

	con_vector<thread*> threads;
	bool isConstructed = true;
	
	
};

namespace count{
	static Counter taskCounter;
	static atomic<int> queueCounter = 0;
}

#endif