#ifndef FIBER_H
#define FIBER_H

#include "Task.h"

#include <atomic>

using std::atomic;

class Fiber{
public:
	Fiber(atomic<int>& counter);
	~Fiber();
	void runAndFree(Task &task);
	bool isFiberFree();
	void run(Task &task);
	void free();
private:
	
	

	Task *currentTask;
	std::atomic<bool> isFree = true;
	atomic<int>* counterPtr;
};

#endif