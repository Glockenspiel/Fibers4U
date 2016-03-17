#include "include\con_data_struct\extern_locker.h"

namespace fbr{

	void extern_locker::get_lock_extern(){
		while (lock.test_and_set(std::memory_order_seq_cst));
		locked_externally = true;
	}


	void extern_locker::unlock_extern(){
		lock.clear(std::memory_order_seq_cst);
		locked_externally = false;
	}

	bool extern_locker::getLockState(){
		return locked_externally.get();
	}
}