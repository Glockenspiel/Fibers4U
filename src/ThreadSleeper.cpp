#include "include/thread_sleeper.h"

namespace fbr{
	void ThreadSleeper::sleep(){
		isAwake = false;
		std::unique_lock<std::mutex> lock(mtx);
		while (isAwake == false)
			cv.wait(lock);
	}

	void ThreadSleeper::wakeUp(){
		if (isAwake == false)
		{
			isAwake = true;
			cv.notify_one();
		}
	}

	bool ThreadSleeper::getIsAwake(){
		return isAwake.get();
	}
}