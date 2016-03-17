#include "include\con_data_struct\extern_locker.h"

namespace fbr{

	void extern_locker::getLock(){
		while (lock.test_and_set(std::memory_order_seq_cst));
		locked_externally = true;
	}


	void extern_locker::unlock(){
		lock.clear(std::memory_order_seq_cst);
		locked_externally = false;
	}

	bool extern_locker::isLocked(){
		return locked_externally.get();
	}
}