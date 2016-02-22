#ifndef SPIN_LOCK_H
#define SPIN_LOCK_H

#include <atomic>
#include "Timer.h"

class SpinLock{
public:
	SpinLock();
	~SpinLock();
	void lock();
	void unlock();
	bool getIsLocked();
private:
	std::atomic<bool> isLocked = false;
	std::time_t startLockTime;
	bool curLock;
	Timer *timer;
};

#endif