#ifndef FIBER_H
#define FIBER_H

#include "Task.h"
#include "SpinLock.h"

#include <atomic>

using std::atomic;

class Fiber{
public:
	Fiber(atomic<int>& counter, unsigned short id);
	~Fiber();
	void runAndFree(Task &task);
	bool isFiberFree();
	void run(Task &task);
	void free();
	void switchOut();
private:
	SpinLock *spinLock;
	unsigned int id;

	Task *currentTask;
	std::atomic<bool> isFree = true;
	atomic<int>* counterPtr;
};

#endif