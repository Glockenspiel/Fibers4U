#ifndef CON_STACK_H
#define CON_STACK_H

#include "include/locker.h"
#include "element.h"

namespace fbr{
	template <class T>
	class con_stack : public Locker{
	public:
		void push(T t);
		T top();
		void pop();
		T getPop();
		bool empty();
		int size();

		//unsyncronized versions
		//--------------------------
		void push_unsync(T t);
		T top_unsync();
		void pop_unsync();
		T getPop_unsync();
		bool empty_unsync();
		int size_unsync();
		//--------------------------

	private:
		element<T>	*top_ptr = nullptr,
			*current = nullptr;
	};

	template <class T>
	void con_stack<T>::push(T t){
		getLock();
		push_unsync(t);
		unlock();
	}

	template <class T>
	void con_stack<T>::pop(){
		getLock();
		top_ptr = pop_unsync();
		unlock();
	}

	template <class T>
	T con_stack<T>::top(){
		T t;
		getLock();
		t = top_unsync();
		unlock();
		return t;
	}

	template <class T>
	T con_stack<T>::getPop(){
		T t;
		getLock();
		top_ptr = getPop_unsync();
		unlock();
		return t;
	}

	template <class T>
	bool con_stack<T>::empty(){
		bool flag;
		getLock();
		flag = empty_unsync();
		unlock();
		return flag;
	}

	template <class T>
	int con_stack<T>::size(){
		int count = 0;
		getLock();
		count = size_unsync();
		unlock();
		return count;
	}

	//------------------------
	//unsyncronized functions
	//------------------------

	template <class T>
	void con_stack<T>::push_unsync(T t){
		element<T> *e = new element<T>();
		e->next = nullptr;
		e->val = t;

		if (top_ptr == nullptr)
		{
			top_ptr = e;
		}
		else
		{
			e->next = top_ptr;
			top_ptr = e;
		}
	}

	template <class T>
	void con_stack<T>::pop_unsync(){
		top_ptr = top_ptr->next;
	}

	template <class T>
	T con_stack<T>::top_unsync(){
		return top_ptr->val;
	}

	template <class T>
	T con_stack<T>::getPop_unsync(){
		T t;
		t = top_ptr->val;
		top_ptr = top_ptr->next;
		return t;
	}

	template <class T>
	bool con_stack<T>::empty_unsync(){
		return top_ptr == nullptr;
	}

	template <class T>
	int con_stack<T>::size_unsync(){
		int count = 0;
		current = top_ptr;

		do {
			current = current->next;
			count++;
		} while (current != nullptr);
		return count;
	}
}

#endif