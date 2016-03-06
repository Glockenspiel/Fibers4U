#include "SpinLock.h"
#include <iostream>
#include "Timer.h"
#include "con_iostream.h"
#include <thread>


SpinLock::SpinLock(){}

SpinLock::~SpinLock(){}

//begin spin lock
void SpinLock::acquireLock(){
	while (isLocked.test_and_set(std::memory_order_seq_cst));
	curLock.store(true, std::memory_order_relaxed);
}

//unlocks the spin lock from another thread
void SpinLock::unlock(){
	isLocked.clear(std::memory_order_seq_cst);
	curLock.store(false, std::memory_order_relaxed);
}

bool SpinLock::getIsLocked(){
	return curLock;
}