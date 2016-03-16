#ifndef CON_STACK_H
#define CON_STACK_H

#include <atomic>
#include "element.h"

namespace fbr{
	template <class T>
	class con_stack{
	public:
		void push(T t);
		T top();
		void pop();
		T getPop();
		bool empty();
		int size();

	private:
		void getLock();
		void unlock();

		element<T>	*top_ptr = nullptr,
			*current = nullptr;

		std::atomic_flag lock;
	};

	template <class T>
	void con_stack<T>::push(T t){
		element<T> *e = new element<T>();
		e->next = nullptr;
		e->val = t;

		getLock();
		if (top_ptr == nullptr)
		{
			top_ptr = e;
		}
		else
		{
			e->next = top_ptr;
			top_ptr = e;
		}

		fbr::con_cout << "Pushed: " << top_ptr->val << "    Next: ";
		if (top_ptr->next != nullptr)
			fbr::con_cout << top_ptr->next->val << fbr::endl;
		else
			fbr::con_cout << fbr::endl;
		unlock();
	}

	template <class T>
	void con_stack<T>::pop(){
		getLock();
		top_ptr = top_ptr->next;
		unlock();
	}

	template <class T>
	T con_stack<T>::top(){
		T t;
		getLock();
		t = top_ptr->val;
		unlock();
		return t;
	}

	template <class T>
	T con_stack<T>::getPop(){
		T t;
		getLock();
		t = top_ptr->val;
		top_ptr = top_ptr->next;
		unlock();
		return t;
	}

	template <class T>
	bool con_stack<T>::empty(){
		bool flag;
		getLock();
		flag = top_ptr == nullptr;
		unlock();
		return flag;
	}

	template <class T>
	int con_stack<T>::size(){
		int count = 0;
		getLock();
		current = top_ptr;

		do {
			current = current->next;
			count++;
		} while (current != nullptr);
		unlock();
		return count;
	}

	template <class T>
	void con_stack<T>::getLock(){
		while (lock.test_and_set(std::memory_order_seq_cst));
	}

	template <class T>
	void con_stack<T>::unlock(){
		lock.clear(std::memory_order_seq_cst);
	}
}

#endif