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
static std::atomic_flag writeFlag = ATOMIC_FLAG_INIT;


class global{
public:
	static void writeLock(){
		while (writeFlag.test_and_set(std::memory_order_seq_cst));
		//while (mtx->try_lock() == false){}
	}

	static void writeUnlock(){
		//mtx->unlock();
		writeFlag.clear(std::memory_order_seq_cst);
	}

	static unsigned int getThreadCount(){
		return std::thread::hardware_concurrency();
	}

};

namespace fbr{
class Log
{
public:
	//standard log
	Log(){
		getLock();
	}

	//Warning logger, prints the function name too
	Log(const std::string &funcName){
		getLock();
		std::cout << "Warning! "<< funcName << ": "  << std::endl;
	}

	//spinlock until lock is acquired
	void getLock(){
		while (mtx->try_lock() == false){}
	}

	//print the next value
	template <class T>
	Log &operator<<(const T &v){
		std::cout << v;
		return *this;
	}

	// function that takes a custom stream, and returns it
	typedef Log& (*LogManipulator)(Log&);

	//take a function with the typedef signature
	Log& operator<<(LogManipulator manip){
		return manip(*this);
	}

	

	//release lock after printing has completed
	~Log(){
		mtx->unlock();
	}
};

//custom endl for character stream using inline function
inline Log& endl(Log& log){

	std::cout << std::endl;

	return log;
}

//preprocessor directives for cout and cout_warn
#define cout Log()
#define cout_warn Log(__FUNCTION__)
}

//using namespace fbr when including this header file
using namespace fbr;


#endif