#ifndef WORKER_H
#define WORKER_H

#include "Fiber.h"
#include <atomic>
#include <mutex>
#include <chrono>

using std::atomic;
using namespace std::chrono;

class Worker{
public:
	enum State {free,aquired, prepared};
	Worker(unsigned int id);
	~Worker();
	void run();
	void switchFiber(Fiber& fiber);
	void close();
	void set(Fiber& fiber);
	bool tryAcquire();
	unsigned int getID();
	void forceAcquire();
private:
	atomic<bool> running = false;
	Fiber* currentFiber;
	atomic<State> state = free;
	void setState(State s);
	unsigned int id;
	high_resolution_clock::time_point lastRun, timeNow;

	duration<double, std::milli> 
		timeSinceLastRun, 
		timeUntilSleep = duration<double, std::milli>(1000);
};

#endif