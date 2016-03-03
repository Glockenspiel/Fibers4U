#ifndef SPIN_LOCK_H
#define SPIN_LOCK_H

#include <atomic>
#include "Timer.h"

class SpinLock{
public:
	SpinLock();
	~SpinLock();
	//begins the spin lock
	void lock();

	//unlocks the spinlock (must be called by another thread)
	void unlock();

	//returns true if locked
	bool getIsLocked();
private:
	std::atomic<bool> isLocked = false;
	bool curLock;
	Timer *timer;
};

#endif