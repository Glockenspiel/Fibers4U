#ifndef FIBER_H
#define FIBER_H

#include "BaseTask.h"
#include "SpinLock.h"

#include <atomic>

//namspace for prioity enum so it can be accessed more easily
namespace priority{
	enum Priority { low, medium, high };
}

using std::atomic;
using namespace priority;



class Fiber{
public:
	enum State { free, acquired, prepared };

	Fiber(atomic<int>& counter, unsigned short id);
	~Fiber();

	void runAndFree();
	void run();
	void freeTask();
	void setTask(BaseTask* task, priority::Priority p);
	unsigned int getID();
	void setPrepared();
	void waitUntilFree();
	bool inState(State s);
	bool tryAcquire();
	priority::Priority getPriority();
	
private:
	void waitForState(State);
	void setState(State);
	SpinLock *spinLock;
	unsigned int id;
	atomic<State> state = free;
	BaseTask *currentTask;
	atomic<int>* counterPtr;
	Priority currentPriority = Priority::low;
};



#endif