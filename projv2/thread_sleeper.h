#ifndef THREAD_SLEEPER_H
#define THREAD_SLEEPER_H

#include <mutex>
#include <condition_variable>
#include "concurrent.h"

class ThreadSleeper{
public:
	//locks current thread with a mutex until it is woken up
	void sleep();

	//wake up the locked thread from sleeping
	void wakeUp();
	
	//if thread is awake returns true
	bool getIsAwake();
private:
	std::condition_variable cv;
	std::mutex mtx;
	concurrent<bool> isAwake = true;
};

#endif