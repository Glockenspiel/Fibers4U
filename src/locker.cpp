#include "include/locker.h"

namespace fbr{

	void Locker::getLock(){
		while (lock.test_and_set(std::memory_order_seq_cst));
		locked_externally = true;
	}


	void Locker::unlock(){
		lock.clear(std::memory_order_seq_cst);
		locked_externally = false;
	}

	bool Locker::isLocked(){
		return locked_externally.get();
	}
}