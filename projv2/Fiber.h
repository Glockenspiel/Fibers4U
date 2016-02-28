#ifndef FIBER_H
#define FIBER_H

#include "BaseTask.h"
#include "SpinLock.h"

#include <atomic>

using std::atomic;

class Fiber{
public:
	enum State { free, acquired, prepared };
	enum Priority { low, medium, high };

	Fiber(atomic<int>& counter, unsigned short id);
	~Fiber();

	void runAndFree();
	void run();
	void freeTask();
	void setTask(BaseTask* task, Priority p);
	unsigned int getID();
	void setPrepared();
	void waitUntilFree();
	bool inState(State s);
	bool tryAcquire();
	
	
private:
	void waitForState(State);
	void setState(State);
	SpinLock *spinLock;
	unsigned int id;
	atomic<State> state = free;
	BaseTask *currentTask;
	atomic<int>* counterPtr;
	Priority priority = Priority::medium;
};

#endif