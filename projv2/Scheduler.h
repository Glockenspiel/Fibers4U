#ifndef SCHEDULER_H
#define SCHEDULER_H

#include "Task.h"
#include <vector>
#include <atomic>
#include "Fiber.h"
#include <thread>
#include "SpinLock.h"
#include "Global.h"
#include "Worker.h"

using std::thread;
using std::vector;

static atomic<bool> isCompleted = false;

class Scheduler{
public:
	Scheduler(unsigned const int FIBER_COUNT, unsigned const int THREAD_COUNT, Task& startingTask);
	~Scheduler();
	void runTask(Task &task);
	void close();
	bool getIsConstructed();
	bool getEndProcess();
	void Scheduler::waitAllFibersFree();
	
private:
	vector<Fiber *> fibers;
	vector<Worker *> workers;
	vector<thread*> threads;
	atomic<int> counter=0;
	unsigned const int  *N_FIBER_PTR, *N_THREAD_PTR;
	bool isConstructed = true;
	atomic<bool> endProcess = false;
	void empty();
	Fiber* acquireFreeFiber();
	Worker* acquireFreeWorker();
};

#endif