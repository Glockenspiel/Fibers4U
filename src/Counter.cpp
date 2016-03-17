#include "include/con_data_struct/Counter.h"

namespace fbr{
	void Counter::add(int a){
		while (lock.test_and_set(std::memory_order_seq_cst));
		count += a;
		lock.clear(std::memory_order_seq_cst);
	}

	void Counter::sub(int a){
		while (lock.test_and_set(std::memory_order_seq_cst));
		count -= a;
		lock.clear(std::memory_order_seq_cst);
	}

	int Counter::get(){
		int a;
		while (lock.test_and_set(std::memory_order_seq_cst));
		a = count;
		lock.clear(std::memory_order_seq_cst);
		return a;
	}
}