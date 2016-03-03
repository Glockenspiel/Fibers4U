#ifndef CON_VECTOR_H
#define CON_VECTOR_H

#include <vector>
#include <atomic>

//concurrent data structure for std::vector which uses atomic spinlocks
template<class T>
class con_vector{
public:
	//pushes item to back of the vector
	void push_back(T t);

	//returns the size of the vector
	unsigned int size();

	//returns true if the vector is empty
	bool empty();

	//erases at the index
	void erase(int index);

	//returns the item at index
	T at(int index);

	//operator overloading for element access
	T operator[](int index);

	//clears the list
	void clear();

private:
	std::vector<T> list;
	std::atomic_flag lock;
};

template<class T>
unsigned int con_vector<T>::size(){
	unsigned int a;
	while (lock.test_and_set(std::memory_order_seq_cst));
	a = list.size();
	lock.clear(std::memory_order_seq_cst);
	return a;
}

template<class T>
void con_vector<T>::push_back(T t){
	while (lock.test_and_set(std::memory_order_seq_cst));
	list.push_back(t);
	lock.clear(std::memory_order_seq_cst);
}

template<class T>
bool con_vector<T>::empty(){
	bool flag;
	while (lock.test_and_set(std::memory_order_seq_cst));
	flag = list.empty();
	lock.clear(std::memory_order_seq_cst);
	return flag;
}

template<class T>
void con_vector<T>::erase(int index){
	while (lock.test_and_set(std::memory_order_seq_cst));
	list.erase(list.begin() + index);
	lock.clear(std::memory_order_seq_cst);
}

template<class T>
T con_vector<T>::at(int index){
	T t;
	while (lock.test_and_set(std::memory_order_seq_cst));
	t = list.at(index);
	lock.clear(std::memory_order_seq_cst);
	return t;
}

template<class T>
T con_vector<T>::operator[](int index){
	return at(index);
}

template <class T>
void con_vector<T>::clear(){
	while (lock.test_and_set(std::memory_order_seq_cst));
	list.clear();
	lock.clear(std::memory_order_seq_cst);
}

#endif