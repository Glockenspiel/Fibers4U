#ifndef SPIN_LOCK_H
#define SPIN_LOCK_H

#include <atomic>

class SpinLock{
public:
	SpinLock();
	~SpinLock();

	//begins the spin lock
	void acquireLock();

	//unlocks the spinlock (must be called by another thread)
	void unlock();

	//returns true if locked
	bool getIsLocked();
private:
	std::atomic_flag isLocked;
	std::atomic<bool> curLock;
};

#endif