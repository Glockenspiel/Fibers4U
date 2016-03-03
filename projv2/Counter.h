#ifndef COUNTER_H
#define COUTNER_H

#include <atomic>

class Counter{
public:
	void add(int a);
	void sub(int a);
	int get();

	//Operater overloading
	Counter& operator++(){
		add(1);
		return *this;
	}

	Counter& operator--(){
		sub(1);
		return *this;
	}

	Counter& operator+=(int a){
		add(a);
		return *this;
	}

	Counter& operator-=(int a){
		sub(a);
		return *this;
	}

private:
	int count;
	std::atomic_flag lock;
};

#endif