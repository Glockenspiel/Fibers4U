#ifndef CON_QUEUE_H
#define CON_QUEUE_H

#include "element.h"
#include "include/locker.h"

namespace fbr{
	//concurrent data strcuture using std::queue which uses FIFO and atomic spinlocks (first in first out)
	template<class T>
	class con_queue : public Locker{
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

		//asyncronous versions to be used with extern_locker
		//---------------------------------------------------
		T getPop_async();
		void pop_async();
		T front_async();
		T back_async();
		void push_async(T t);
		int size_async();
		bool empty_async();
		//---------------------------------------------------

	private:
		element<T>	*frontptr = nullptr,
			*backptr = nullptr;
	};

	template<class T>
	T con_queue<T>::getPop(){
		T t;
		getLock();
		t = getPop_async();
		unlock();
		return t;
	}

	//pop function with locking
	template<class T>
	void con_queue<T>::pop(){
		getLock();
		getPop_async();
		unlock();
	}


	template<class T>
	T con_queue<T>::front(){
		T t;
		getLock();
		t = front_async();
		unlock();
		return t;
	}

	template<class T>
	void con_queue<T>::push(T t){
		getLock();
		push_async(t);
		unlock();
	}

	template<class T>
	int con_queue<T>::size(){
		int count = 0;
		getLock();
		count = size_async();
		unlock();
		return count;
	}

	template<class T>
	bool con_queue<T>::empty(){
		bool flag;
		getLock();
		flag = empty_async();
		unlock();
		return flag;
	}

	template<class T>
	T con_queue<T>::back(){
		T t;
		getLock();
		t = back_async();
		unlock();
		return t;
	}

	//----------------------------------------------------------------
	//asyncronous functions
	//----------------------------------------------------------------

	template<class T>
	T con_queue<T>::getPop_async(){
		T t;
		t = frontptr->val;
		frontptr = frontptr->next;
		return t;
	}

	//pop function with locking
	template<class T>
	void con_queue<T>::pop_async(){
		frontptr = frontptr->next;
	}


	template<class T>
	T con_queue<T>::front_async(){
		return frontptr->val;
	}

	template<class T>
	void con_queue<T>::push_async(T t){
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
	int con_queue<T>::size_async(){
		int count = 0;
		element<T> *ptr = frontptr;

		do {
			ptr = ptr->next;
			count++;
		} while (ptr != nullptr);

		return count;
	}

	template<class T>
	bool con_queue<T>::empty_async(){
		return frontptr == nullptr;
	}

	template<class T>
	T con_queue<T>::back_async(){
		return = back->val;
	}
}

#endif