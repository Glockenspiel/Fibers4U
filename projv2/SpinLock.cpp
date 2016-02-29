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

	while (isLocked.load(std::memory_order_relaxed)){}
}

//unlocks the spin lock
void SpinLock::unlock(){
	isLocked.store(false, std::memory_order_release);
}

bool SpinLock::getIsLocked(){
	return isLocked.load(std::memory_order_relaxed);
}