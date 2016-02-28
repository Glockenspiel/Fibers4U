#ifndef WORKER_H
#define WORKER_H

#include "Fiber.h"
#include <atomic>
#include <mutex>

using std::atomic;

class Worker{
public:
	enum State {free,aquired, prepared};
	Worker(unsigned int id);
	~Worker();
	void run();
	void nextTask(Task& task);
	void switchFiber(Fiber& fiber);
	void close();
	void set(Task& task, Fiber& fiber);
	void set(Fiber& fiber);
	bool tryAcquire();
	unsigned int getID();
	void forceAcquire();
private:
	atomic<bool> running = false;
	Task* nextTaskPtr;
	Fiber* currentFiber;
	atomic<State> state = free;
	void setState(State s);
	unsigned int id;
};

#endif