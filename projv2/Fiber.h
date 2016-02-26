#ifndef FIBER_H
#define FIBER_H

#include "Task.h"
#include "SpinLock.h"

#include <atomic>

using std::atomic;

class Fiber{
public:
	enum State { running, freed, prepared };
	Fiber(atomic<int>& counter, unsigned short id);
	~Fiber();
	void runAndFree(Task &task);
	//bool isFiberFree();
	void run(Task &task);
	void free();
	void setTask(Task& task);
	unsigned int getID();
	void setPrepared();
	void waitUntilFree();
private:
	void waitForState(State);
	void setState(State);
	SpinLock *spinLock;
	unsigned int id;
	atomic<State> state = freed;
	Task *currentTask;
	//std::atomic<bool> isFree = true;
	atomic<int>* counterPtr;
};

#endif