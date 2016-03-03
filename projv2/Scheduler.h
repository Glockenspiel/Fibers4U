#ifndef SCHEDULER_H
#define SCHEDULER_H

#include "BaseTask.h"
#include <vector>
#include <atomic>
#include "Fiber.h"
#include <thread>
#include "SpinLock.h"
#include "Global.h"
#include "Worker.h"
#include <mutex>
#include <condition_variable>
#include <queue>
#include "FiberPool.h"
#include "WaitingTask.h"
#include "Counter.h"


using std::thread;
using std::vector;
using std::mutex;
using std::queue;

static atomic<bool> isCompleted = false;
static FiberPool *fiberPool;
static mutex *mainMtx;
static std::condition_variable mainCV;

static vector<WaitingTask *> waitingTasks;
static vector<Worker *> workers;


//atomic flag for accessing waitingTasks
static std::atomic_flag waitingLock = ATOMIC_FLAG_INIT;

class Scheduler{
public:
	Scheduler(unsigned const int FIBER_COUNT, unsigned const int THREAD_COUNT, 
		BaseTask* startingTask);
	~Scheduler();
	
	static void runTasks(vector<BaseTask*> tasks, Priority taskPriority);
	void close();
	bool getIsConstructed();
	static void waitAllFibersFree();

	static void waitForCounter(unsigned int count, BaseTask* task);
	static void waitForCounter(WaitingTask& task);

	static void checkWaitingTasks();
	static void wakeUpMain();
	static void waitMain();
	static void notifyWorkerBeenFreed(Worker* worker);
	static void taskFinished();
	
private:
	
	vector<thread*> threads;
	static void addToQueue(BaseTask *task, priority::Priority taskPrioirty);
	unsigned const int  *N_FIBER_PTR, *N_THREAD_PTR;
	bool isConstructed = true;
	static void empty();
	static Worker* acquireFreeWorker();
	
};

namespace count{
	static Counter taskCounter;
	static atomic<int> queueCounter = 0;
}

#endif