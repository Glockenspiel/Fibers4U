#ifndef CON_CONTAINER_H
#define CON_CONTAINER_H

#include <atomic>
#include "include/con_data_struct/concurrent.h"

namespace fbr{
	/*
	creates a spin lock which can be used on its own or inherited by a class.
	
	for example con_queue, con_stack and con_vector inherit this class so
	they can be locked externally before calling their unsyncronized functions
	and then unlocked externally after. 
	
	Note: This class makes use of using sequentially consistent ordering 
		i.e. std::memory_order_seq_con
	*/
	class extern_locker{
	public:
		//gets the lock 
		virtual void get_lock_extern();

		//releases the lock
		virtual void unlock_extern();

		//returns the current state of the lock
		virtual bool getLockState();

	private:
		concurrent<bool> locked_externally = false;
		std::atomic_flag lock;
	};
}
#endif