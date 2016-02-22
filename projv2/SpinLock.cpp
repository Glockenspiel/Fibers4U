#include "SpinLock.h"
#include <iostream>
#include "Timer.h"

std::atomic_flag lockflag = ATOMIC_FLAG_INIT;

SpinLock::SpinLock(){
	timer = new Timer();
}

SpinLock::~SpinLock(){

}

//begin spin lock
void SpinLock::lock(){
	isLocked = true;

	curLock = true;

	//start spin lock
	while (curLock){
		//atomic operation
		lockflag.test_and_set(std::memory_order_acquire);
			if (isLocked == false){
				curLock = false;
			}
		lockflag.clear(std::memory_order_release);

		//timer->wait(1);
	}
}

//unlocks the spin lock
void SpinLock::unlock(){
	lockflag.test_and_set(std::memory_order_acquire);
		isLocked = false;
		std::cout << "unlocked\n";
 	lockflag.clear(std::memory_order_release);
}

bool SpinLock::getIsLocked(){
	return isLocked;
}