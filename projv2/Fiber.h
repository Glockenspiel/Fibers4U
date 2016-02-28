#ifndef FIBER_H
#define FIBER_H

#include "BaseTask.h"
#include "SpinLock.h"

#include <atomic>

using std::atomic;

class Fiber{
public:
	enum State { freed, prepared, acquired };
	Fiber(atomic<int>& counter, unsigned short id);
	~Fiber();
	void runAndFree();
	void run();
	void free();
	void setTask(BaseTask* task);
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
	atomic<State> state = freed;
	BaseTask *currentTask;
	atomic<int>* counterPtr;
};

#endif