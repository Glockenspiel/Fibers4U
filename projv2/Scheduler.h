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


using std::thread;
using std::vector;
using std::mutex;
using std::queue;

static atomic<bool> isCompleted = false;
static FiberPool *fiberPool;

class Scheduler{
public:
	Scheduler(unsigned const int FIBER_COUNT, unsigned const int THREAD_COUNT, 
		BaseTask* startingTask);
	~Scheduler();
	void runTask(BaseTask* task);
	void runTask(BaseTask *task, priority::Priority taskPrioirty);
	void runTasks(vector<BaseTask*> tasks);
	void runTasks(vector<BaseTask*> tasks, Priority taskPriority);
	void close();
	bool getIsConstructed();
	void Scheduler::waitAllFibersFree();
	void wakeUpMain();
	void waitMain();
	static void workerBeenFreed(Worker* worker);
	
private:
	vector<Worker *> workers;
	vector<thread*> threads;
	atomic<int> counter=0;
	unsigned const int  *N_FIBER_PTR, *N_THREAD_PTR;
	bool isConstructed = true;
	void empty();
	Worker* acquireFreeWorker();
	mutex *mtx;
	
	std::condition_variable mainCV;
};

#endif