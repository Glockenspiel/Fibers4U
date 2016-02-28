#ifndef GLOBAL_H
#define GLOBAL_H

#include <mutex>
#include <iostream>
#include <string>
#include <atomic>
#include <thread>
#include <string>
#include <iostream>
#include <stdarg.h>

using std::string;

static std::mutex *mtx = new std::mutex;
static std::atomic<int> RWCounter = 0;



class global{
public:
	static void writeLock(){
		while (mtx->try_lock() == false){}
	}

	static void writeUnlock(){
		mtx->unlock();
	}

	static unsigned int getThreadCount(){
		return std::thread::hardware_concurrency();
	}
};




#endif