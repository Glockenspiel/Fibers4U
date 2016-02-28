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
#include <mutex>
#include <condition_variable>


using std::thread;
using std::vector;
using std::mutex;


static atomic<bool> isCompleted = false;

class Scheduler{
public:
	Scheduler(unsigned const int FIBER_COUNT, unsigned const int THREAD_COUNT, 
		Task& startingTask);
	~Scheduler();
	void runTask(Task &task);
	void close();
	bool getIsConstructed();
	void Scheduler::waitAllFibersFree();
	void wakeUpMain();
	void waitMain();
	static void workerBeenFreed(Worker* worker);
private:
	vector<Fiber *> fibers;
	vector<Worker *> workers;
	vector<thread*> threads;
	
	atomic<int> counter=0;
	unsigned const int  *N_FIBER_PTR, *N_THREAD_PTR;
	bool isConstructed = true;
	void empty();
	Fiber* acquireFreeFiber();
	Worker* acquireFreeWorker();
	mutex *mtx;
	
	std::condition_variable mainCV;
};

#endif