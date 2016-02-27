#ifndef WORKER_H
#define WORKER_H

#include "Fiber.h"
#include <atomic>
#include <mutex>

using std::atomic;

class Worker{
public:
	enum State {free,aquired, prepared};
	Worker();
	~Worker();
	void run();
	void nextTask(Task& task);
	void switchFiber(Fiber& fiber);
	void close();
	void set(Task& task, Fiber& fiber);
	bool tryAcquire();
private:
	atomic<bool> running = false;
	Task* nextTaskPtr;
	Fiber* currentFiber;
	atomic<State> state = free;
	void setState(State s);
};

#endif