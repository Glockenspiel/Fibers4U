#include "include/con_data_struct/Counter.h"


namespace fbr{
	Counter::Counter(std::string name){
		this->name = name;
	}

	void Counter::add(int a){
		getLock();
			count += a;
		unlock();
	}

	void Counter::sub(int a){
		getLock();
		count -= a;
		unlock();
	}

	int Counter::get(){
		int a;
		getLock();
			a = count;
		unlock();
		return a;
	}

	std::string Counter::getName(){
		std::string val;
		getLock();
			val = name;
		unlock();
		return val;
	}

	void Counter::getLock(){
		while (lock.test_and_set(std::memory_order_seq_cst));
	}

	void Counter::unlock(){
		lock.clear(std::memory_order_seq_cst);
	}
}