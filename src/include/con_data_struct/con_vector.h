#ifndef CON_VECTOR_H
#define CON_VECTOR_H

//#include <vector>
//#include <atomic>

#include "element.h"
//#include "concurrent.h"
#include "include/con_data_struct/extern_locker.h"

namespace fbr{
	//concurrent data structure for std::vector which uses atomic spinlocks
	template<class T>
	class con_vector : public extern_locker{
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


		//These functions allows the vector to be accessed unsyncronized
		//This allows for the freedom of easily switching between 
		//a synchronized and unsynchronized vector
		//to be used with get_lock_extern() and unlock_extern() from the extern_locker class
		//---------------------------------------------------------------------------
		void push_back_unsync(T t);
		unsigned int size_unsync();
		bool empty_unsync();
		void erase_unsync(unsigned int index);
		T at_unsync(unsigned int index);
		void clear_unsync();
		//---------------------------------------------------------------------------


	private:
		element<T>	*front = nullptr,
			*back = nullptr,
			*cur = nullptr;

		unsigned int totalSize = 0;


	};

	
	template<class T>
	T con_vector<T>::operator[](unsigned int index){
		if (isLocked() == false)
			return at(index);
		else
			return at_unsync(index);
	}



	template<class T>
	unsigned int con_vector<T>::size(){
		unsigned int count;
		getLock();
		count = size_unsync();
		unlock();
		return count;
	}

	template<class T>
	void con_vector<T>::push_back(T t){
		getLock();
		push_back_unsync(t);
		unlock();
	}

	template<class T>
	bool con_vector<T>::empty(){
		bool flag;
		getLock();
		flag = empty_unsync;
		unlock();
		return flag;
	}

	template<class T>
	void con_vector<T>::erase(unsigned int index){
		getLock();
		erase_unsync(index);
		unlock();
	}

	template<class T>
	T con_vector<T>::at(unsigned int index){
		T t;
		getLock();
		t = at_unsync(index);
		unlock();
		return t;
	}

	template <class T>
	void con_vector<T>::clear(){
		getLock();
		clear_unsync();
		unlock();
	}

	//---------------------------------
	//unsyncronized functions
	//---------------------------------



	template<class T>
	unsigned int con_vector<T>::size_unsync(){
		unsigned int count;
		count = totalSize;
		return count;
	}

	template<class T>
	void con_vector<T>::push_back_unsync(T t){
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
		totalSize++;
	}

	template<class T>
	bool con_vector<T>::empty_unsync(){
		bool flag;
		flag = front == nullptr;
		return flag;
	}

	template<class T>
	void con_vector<T>::erase_unsync(unsigned int index){
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
		totalSize--;
	}

	template<class T>
	T con_vector<T>::at_unsync(unsigned int index){
		T t;

		cur = front;


		while (index > 0){
			cur = cur->next;
			index--;
		}

		t = cur->val;

		return t;
	}

	template <class T>
	void con_vector<T>::clear_unsync(){
		front = nullptr;
		back = nullptr;
		totalSize = 0;
	}
}

#endif