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

		//asyncronous versions
		//--------------------------
		void push_async(T t);
		T top_async();
		void pop_async();
		T getPop_async();
		bool empty_async();
		int size_async();
		//--------------------------

	private:
		element<T>	*top_ptr = nullptr,
			*current = nullptr;
	};

	template <class T>
	void con_stack<T>::push(T t){
		getLock();
		push_async(t);
		unlock();
	}

	template <class T>
	void con_stack<T>::pop(){
		getLock();
		top_ptr = pop_async();
		unlock();
	}

	template <class T>
	T con_stack<T>::top(){
		T t;
		getLock();
		t = top_async();
		unlock();
		return t;
	}

	template <class T>
	T con_stack<T>::getPop(){
		T t;
		getLock();
		top_ptr = getPop_async();
		unlock();
		return t;
	}

	template <class T>
	bool con_stack<T>::empty(){
		bool flag;
		getLock();
		flag = empty_async();
		unlock();
		return flag;
	}

	template <class T>
	int con_stack<T>::size(){
		int count = 0;
		getLock();
		count = size_async();
		unlock();
		return count;
	}

	//------------------------
	//asyncronous functions
	//------------------------

	template <class T>
	void con_stack<T>::push_async(T t){
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
	void con_stack<T>::pop_async(){
		top_ptr = top_ptr->next;
	}

	template <class T>
	T con_stack<T>::top_async(){
		return top_ptr->val;
	}

	template <class T>
	T con_stack<T>::getPop_async(){
		T t;
		t = top_ptr->val;
		top_ptr = top_ptr->next;
		return t;
	}

	template <class T>
	bool con_stack<T>::empty_async(){
		return top_ptr == nullptr;
	}

	template <class T>
	int con_stack<T>::size_async(){
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