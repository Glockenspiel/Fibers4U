#ifndef CON_VECTOR_H
#define CON_VECTOR_H

#include <vector>
#include <atomic>

#include "element.h"

//concurrent data structure for std::vector which uses atomic spinlocks
template<class T>
class con_vector{
public:
	con_vector(){}
	con_vector(std::initializer_list<T> elems){
		for (T t : elems)
			push_back(t);
	}

	//pushes item to back of the vector
	void push_back(T t);

	//returns the size of the vector
	unsigned int size();

	//returns true if the vector is empty
	bool empty();

	//erases at the index
	void erase(unsigned int index);

	//returns the item at index
	T at(unsigned int index);

	//operator overloading for element access
	T operator[](unsigned int index);

	//clears the list
	void clear();

private:
	element<T>	*front = nullptr,
				*back = nullptr,
				*cur = nullptr;

	std::vector<T> list;
	std::atomic_flag lock;
};

template<class T>
unsigned int con_vector<T>::size(){
	unsigned int count;
	while (lock.test_and_set(std::memory_order_seq_cst));

	if (front == nullptr){
		count = 0;
	}
	else{
		cur = front;
		count = 1;

		while (cur->next != nullptr){
			cur = cur->next;
			count++;
		}
	}

	lock.clear(std::memory_order_seq_cst);
	return count;
}

template<class T>
void con_vector<T>::push_back(T t){
	while (lock.test_and_set(std::memory_order_seq_cst));
	element<T>* e = new element<T>();
	e->val = t;
	e->next = nullptr;

	if (front == nullptr){
		front = e;
		back = e;
	}
	else{
		back->next = e;
		back = e;
	}

	lock.clear(std::memory_order_seq_cst);
}

template<class T>
bool con_vector<T>::empty(){
	bool flag;
	while (lock.test_and_set(std::memory_order_seq_cst));
	flag = front == nullptr;
	lock.clear(std::memory_order_seq_cst);
	return flag;
}

template<class T>
void con_vector<T>::erase(unsigned int index){
	while (lock.test_and_set(std::memory_order_seq_cst));

	//remove front element
	if (index == 0){
		//keep back up to date if only 1 element
		if (front == back)
			back = nullptr;

		front = front->next;
	}
	else{
		cur = front;

		//get element before the element we want to delete
		while (index > 1){
			cur = cur->next;
			index--;
		}
		//if index not out of range
		if (cur != nullptr){
			//erasing the last element
			if (cur->next == back){
				back = cur;
				cur->next = nullptr;
			}
			//standard erase
			else{
				cur->next = cur->next->next;
			}
		}
	}

	lock.clear(std::memory_order_seq_cst);
}

template<class T>
T con_vector<T>::at(unsigned int index){
	T t;
	while (lock.test_and_set(std::memory_order_seq_cst));

	cur = front;


	while (index > 0){
		cur = cur->next;
		index--;
	}

	t = cur->val;

	lock.clear(std::memory_order_seq_cst);
	return t;
}

template<class T>
T con_vector<T>::operator[](unsigned int index){
	return at(index);
}

template <class T>
void con_vector<T>::clear(){
	while (lock.test_and_set(std::memory_order_seq_cst));
	front = nullptr;
	back = nullptr;
	lock.clear(std::memory_order_seq_cst);
}

#endif