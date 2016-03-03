#ifndef COUNTER_H
#define COUTNER_H

#include <atomic>

class Counter{
public:
	void add(int a);
	void sub(int a);
	int get();
private:
	int count;
	std::atomic_flag lock;
};

#endif