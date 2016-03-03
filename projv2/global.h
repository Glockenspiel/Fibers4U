#ifndef GLOBAL_H
#define GLOBAL_H

#include <iostream>
#include <string>
#include <atomic>
#include <thread>
#include <string>
#include <iostream>
#include <stdarg.h>

using std::string;

static std::atomic_flag writeFlag = ATOMIC_FLAG_INIT;


class global{
public:
	//acquires the writing lock
	static void writeLock(){
		while (writeFlag.test_and_set(std::memory_order_seq_cst));
	}

	//releases the writing lock
	static void writeUnlock(){
		writeFlag.clear(std::memory_order_seq_cst);
	}

	//returns the thread count found by the run time
	static unsigned int getThreadCount(){
		return std::thread::hardware_concurrency();
	}
};

//namespace for concurrent standard output
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
		while (writeFlag.test_and_set(std::memory_order_seq_cst));
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
		writeFlag.clear(std::memory_order_seq_cst);
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

#endif