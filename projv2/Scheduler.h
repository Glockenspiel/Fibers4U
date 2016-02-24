#ifndef SCHEDULER_H
#define SCHEDULER_H

#include "Task.h"
#include <vector>
#include <atomic>
#include "Fiber.h"
#include <thread>
#include "SpinLock.h"
#include "Global.h"

using std::thread;
using std::vector;

static atomic<bool> isCompleted = false;

class Scheduler{
public:
	Scheduler(unsigned const int FIBER_COUNT, unsigned const int THREAD_COUNT);
	~Scheduler();
	void runTask(Task &task);
	void close();
	bool getIsConstructed();
	void waitForThreadsFreed();
	bool getEndProcess();
	
private:
	vector<Fiber *> fibers;
	vector<Task*> queuedTasks;
	vector<Task *> tasks;
	atomic<int> counter=0;
	vector<thread*> threads;
	vector<SpinLock*> spinLocks;
	unsigned const int  *N_FIBER_PTR, *N_THREAD_PTR;
	bool isConstructed = true;
	atomic<bool> endProcess = false;
};

#endif