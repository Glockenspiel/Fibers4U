#include "SpinLock.h"
#include <iostream>
#include "Timer.h"
#include "Global.h"
#include <thread>

std::atomic_flag lockflag = ATOMIC_FLAG_INIT;

SpinLock::SpinLock(){
	timer = new Timer();
}

SpinLock::~SpinLock(){

}

//begin spin lock
void SpinLock::lock(){
	isLocked = true;

	//std::this_thread::yield();
	//while (true){}
	while (isLocked.load(std::memory_order_relaxed)){}
	/*
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
	*/
}

//unlocks the spin lock
void SpinLock::unlock(){
	//lockflag.test_and_set(std::memory_order_acquire);
	isLocked.store(false, std::memory_order_release);

		global::writeLock();
		std::cout << "Spin lock ended" << std::endl;
		global::writeUnlock();

 	//lockflag.clear(std::memory_order_release);
}

bool SpinLock::getIsLocked(){
	return isLocked.load(std::memory_order_relaxed);
}