#ifndef CON_CONTAINER_H
#define CON_CONTAINER_H

#include <atomic>
#include "include/con_data_struct/concurrent.h"

namespace fbr{
	/*
	creates a spin lock which can be used by a class or declared by itself to help with syncronization
	
	Note: This class makes use of using sequentially consistent ordering 
		i.e. std::memory_order_seq_cst
	*/
	class extern_locker{
	public:
		//gets the lock 
		virtual void getLock();

		//releases the lock
		virtual void unlock();

		//returns the current state of the lock
		virtual bool isLocked();

	private:
		//value for the state as atomic_flag doesn't return a value of it's state
		concurrent<bool> locked_externally = false;
	protected:
		std::atomic_flag lock;
	};
}
#endif