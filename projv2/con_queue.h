#ifndef CON_QUEUE_H
#define CON_QUEUE_H

#include <queue>
#include <atomic>
#include "element.h"

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

	T back();
	//pushes the item onto the back of the queue
	void push(T t);
	//returns the size of the queue
	int size();
	//returns true if the queue is empty
	bool empty();
private:

	//std::queue<T> queue;
	element<T>	*frontptr=nullptr, 
				*backptr=nullptr;

	std::atomic_flag lock;
};

template<class T>
T con_queue<T>::getPop(){
	T t;
	while (lock.test_and_set(std::memory_order_seq_cst));
	t = frontptr->val;
	frontptr = frontptr->next;
	lock.clear(std::memory_order_seq_cst);
	return t;
}

//pop function with locking
template<class T>
void con_queue<T>::pop(){
	while (lock.test_and_set(std::memory_order_seq_cst));
	head = head->next;
	lock.clear(std::memory_order_seq_cst);
}


template<class T>
T con_queue<T>::front(){
	while (lock.test_and_set(std::memory_order_seq_cst));
	return *frontptr;
	lock.clear(std::memory_order_seq_cst);
}

template<class T>
void con_queue<T>::push(T t){
	while (lock.test_and_set(std::memory_order_seq_cst));
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
	lock.clear(std::memory_order_seq_cst);
}

template<class T>
int con_queue<T>::size(){
	int count;
	while (lock.test_and_set(std::memory_order_seq_cst));
	element<T> *ptr = front;
	if (ptr == nullptr)
		count = 0;
	else{
		count = 1;
		while (ptr->next != nullptr){
			ptr = ptr->next;
			count++;
		}
	}
	lock.clear(std::memory_order_seq_cst);
	return count;
}

template<class T>
bool con_queue<T>::empty(){
	bool flag;
	while (lock.test_and_set(std::memory_order_seq_cst));
	flag = frontptr == nullptr;
	lock.clear(std::memory_order_seq_cst);
	return flag;
}

template<class T>
T con_queue<T>::back(){
	return *back;
}



#endif