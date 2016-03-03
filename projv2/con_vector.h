#ifndef CON_VECTOR_H
#define CON_VECTOR_H

#include <vector>
#include <atomic>

//concurrent data structure for std::vector which uses atomic spinlocks
template<class T>
class con_vector{
public:
	//pushes item to back of the vector
	void push_back(T t){
		while (lock.test_and_set(std::memory_order_seq_cst));
		list.push_back(t);
		lock.clear(std::memory_order_seq_cst);
	}

	//returns the size of the vector
	int size(){
		int a;
		while (lock.test_and_set(std::memory_order_seq_cst));
		a = list.size();
		lock.clear(std::memory_order_seq_cst);
		return a;
	}

	//returns true if the vector is empty
	bool empty(){
		bool flag;
		while (lock.test_and_set(std::memory_order_seq_cst));
		flag = list.empty();
		lock.clear(std::memory_order_seq_cst);
		return flag;
	}

	//erases at the index
	void erase(int index){
		while (lock.test_and_set(std::memory_order_seq_cst));
		list.erase(list.begin()+index);
		lock.clear(std::memory_order_seq_cst);
	}

	//returns the item at index
	T at(int index){
		T t;
		while (lock.test_and_set(std::memory_order_seq_cst));
		t = list.at(index);
		lock.clear(std::memory_order_seq_cst);
		return t;
	}

	//operator overloading for at()
	T operator[](int index){
		return at(index);
	}

	//clears the list
	void clear(){
		while (lock.test_and_set(std::memory_order_seq_cst));
		list.clear();
		lock.clear(std::memory_order_seq_cst);
	}

private:
	std::vector<T> list;
	std::atomic_flag lock;
};

#endif