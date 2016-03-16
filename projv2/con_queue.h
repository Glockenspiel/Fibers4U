#ifndef CON_QUEUE_H
#define CON_QUEUE_H

#include <queue>
#include <atomic>
#include "element.h"

namespace fbr{
	//concurrent data strcuture usign std::queue which uses FIFO and atomic spinlocks (first in first out)
	template<class T>
	class con_queue{
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

	private:
		void getLock();
		void unlock();
		//std::queue<T> queue;
		element<T>	*frontptr = nullptr,
			*backptr = nullptr;

		std::atomic_flag lock;
	};

	template<class T>
	T con_queue<T>::getPop(){
		T t;
		getLock();
		t = frontptr->val;
		frontptr = frontptr->next;
		unlock();
		return t;
	}

	//pop function with locking
	template<class T>
	void con_queue<T>::pop(){
		getLock();
		frontptr = frontptr->next;
		unlock();
	}


	template<class T>
	T con_queue<T>::front(){
		getLock();
		return *frontptr;
		unlock();
	}

	template<class T>
	void con_queue<T>::push(T t){
		getLock();
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
		unlock();
	}

	template<class T>
	int con_queue<T>::size(){
		int count = 0;
		getLock();
		element<T> *ptr = frontptr;

		do {
			ptr = ptr->next;
			count++;
		} while (ptr != nullptr);

		unlock();
		return count;
	}

	template<class T>
	bool con_queue<T>::empty(){
		bool flag;
		getLock();
		flag = frontptr == nullptr;
		unlock();
		return flag;
	}

	template<class T>
	T con_queue<T>::back(){
		return *back;
	}

	template <class T>
	void con_queue<T>::getLock(){
		while (lock.test_and_set(std::memory_order_seq_cst));
	}

	template <class T>
	void con_queue<T>::unlock(){
		lock.clear(std::memory_order_seq_cst);
	}
}

#endif