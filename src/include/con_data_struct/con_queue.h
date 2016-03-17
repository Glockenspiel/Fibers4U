#ifndef CON_QUEUE_H
#define CON_QUEUE_H

#include "element.h"
#include "include/con_data_struct/extern_locker.h"

namespace fbr{
	//concurrent data strcuture usign std::queue which uses FIFO and atomic spinlocks (first in first out)
	template<class T>
	class con_queue : public extern_locker{
	public:
		//pops the first element off the queue and returns it
		T getPop();

		//pops off the first element
		void pop();

		//returns the first element
		T front();

		//returns last element
		T back();

		//pushes the item onto the back of the queue
		void push(T t);

		//returns the size of the queue
		int size();

		//returns true if the queue is empty
		bool empty();

		//unsyncronized versions to be used with extern_locker
		//---------------------------------------------------
		T getPop_unsync();
		void pop_unsync();
		T front_unsync();
		T back_unsync();
		void push_unsync(T t);
		int size_unsync();
		bool empty_unsync();
		//---------------------------------------------------

	private:
		void getLock();
		void unlock();

		element<T>	*frontptr = nullptr,
			*backptr = nullptr;

		std::atomic_flag lock;

		concurrent<bool>  locked_externally = false;
	};

	template<class T>
	T con_queue<T>::getPop(){
		T t;
		getLock();
		t = getPop_unsync();
		unlock();
		return t;
	}

	//pop function with locking
	template<class T>
	void con_queue<T>::pop(){
		getLock();
		getPop_unsync();
		unlock();
	}


	template<class T>
	T con_queue<T>::front(){
		T t;
		getLock();
		t = front_unsync();
		unlock();
		return t;
	}

	template<class T>
	void con_queue<T>::push(T t){
		getLock();
		push_unsync(t);
		unlock();
	}

	template<class T>
	int con_queue<T>::size(){
		int count = 0;
		getLock();
		count = size_unsync();
		unlock();
		return count;
	}

	template<class T>
	bool con_queue<T>::empty(){
		bool flag;
		getLock();
		flag = empty_unsync();
		unlock();
		return flag;
	}

	template<class T>
	T con_queue<T>::back(){
		T t;
		getLock();
		t = back_unsync();
		unlock();
		return t;
	}

	template <class T>
	void con_queue<T>::getLock(){
		while (lock.test_and_set(std::memory_order_seq_cst));
	}

	template <class T>
	void con_queue<T>::unlock(){
		lock.clear(std::memory_order_seq_cst);
	}


	//----------------------------------------------------------------
	//unsyncronized versions
	//----------------------------------------------------------------

	template<class T>
	T con_queue<T>::getPop_unsync(){
		T t;
		t = frontptr->val;
		frontptr = frontptr->next;
		return t;
	}

	//pop function with locking
	template<class T>
	void con_queue<T>::pop_unsync(){
		frontptr = frontptr->next;
	}


	template<class T>
	T con_queue<T>::front_unsync(){
		return frontptr->val;
	}

	template<class T>
	void con_queue<T>::push_unsync(T t){
		element<T> *e = new element<T>();
		e->next = nullptr;
		e->val = t;

		//first element on queue
		if (frontptr == nullptr){
			frontptr = backptr = e;
		}
		//standard push
		else{
			backptr->next = e;
			backptr = e;
		}
	}

	template<class T>
	int con_queue<T>::size_unsync(){
		int count = 0;
		element<T> *ptr = frontptr;

		do {
			ptr = ptr->next;
			count++;
		} while (ptr != nullptr);

		return count;
	}

	template<class T>
	bool con_queue<T>::empty_unsync(){
		return frontptr == nullptr;
	}

	template<class T>
	T con_queue<T>::back_unsync(){
		return = back->val;
	}
}

#endif