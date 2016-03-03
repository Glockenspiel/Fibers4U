#ifndef CON_QUEUE_H
#define CON_QUEUE_H

#include <queue>
#include <atomic>

//concurrent data strcuture usign std::queue which uses FIFO and atomic spinlocks (first in first out)
template<class T>
class con_queue{
public:
	//pops the first item off the queue and returns it
	T pop();
	//pushes the item onto the back of the queue
	void push(T t);
	//returns the size of the queue
	int size();
	//returns true if the queue is empty
	bool empty();
private:
	std::queue<T> queue;
	std::atomic_flag lock;
};

template<class T>
T con_queue<T>::pop(){
	T t;
	while (lock.test_and_set(std::memory_order_seq_cst));
	t = queue.front();
	queue.pop();
	lock.clear(std::memory_order_seq_cst);
	return t;
}

template<class T>
void con_queue<T>::push(T t){
	while (lock.test_and_set(std::memory_order_seq_cst));
	queue.push(t);
	lock.clear(std::memory_order_seq_cst);
}

template<class T>
int con_queue<T>::size(){
	int a;
	while (lock.test_and_set(std::memory_order_seq_cst));
	a = queue.size();
	lock.clear(std::memory_order_seq_cst);
	return a;
}

template<class T>
bool con_queue<T>::empty(){
	bool flag;
	while (lock.test_and_set(std::memory_order_seq_cst));
	flag = queue.empty();
	lock.clear(std::memory_order_seq_cst);
	return flag;
}



#endif