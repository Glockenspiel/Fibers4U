#ifndef CONCURRENT_H
#define CONCURRENT_H

#include <atomic>

/*
	wrapper to allow all access and modifications to a class thread safe
	this operations will follow sequentially-consistent ordering according to
	std::memory_order_seq_cst
	This wrapper provide overloading for most of the operators and operands available in C++
*/
template <class T>
class concurrent{
public:
	//constructor
	template <class T2>
	concurrent(T2 t){
		getLock();
		val = t;
		unlock();
	}

	//constructor with reference
	template <class T2>
	concurrent(T2& t){
		getLock();
		val = t;
		unlock();
	}

	//returns the value in the conccurent warppaer
	T& get(){
		getLock();
		T& t = val;
		unlock();
		return t;
	}

	//Assignment operators
	//------------------------------------------------------
	template <class T2>
	void operator=(T2 t){
		getLock();
		val = t;
		unlock();
	}

	template <class T2>
	void operator+=(T2 t2){
		getLock();
		val += t2;
		unlock();
	}

	template <class T2>
	void operator-=(T2 t2){
		getLock();
		val -= t2;
		unlock();
	}

	template <class T2>
	void operator*=(T2 t2){
		getLock();
		val *= t2;
		unlock();
	}

	template <class T2>
	void operator/=(T2 t2){
		getLock();
		val /= t2;
		unlock();
	}
	//------------------------------------------------------



	//Increment and decrement operators
	//------------------------------------------------------
	//postfix
	void operator++(){
		getLock();
		val++;
		unlock();
	}

	//postfix
	void operator--(){
		getLock();
		val--;
		unlock();
	}

	//prefix
	void operator++(int){
		getLock();
		val++;
		unlock();
	}

	//prefix
	void operator--(int){
		getLock();
		val--;
		unlock();
	}

	//------------------------------------------------------


	

	//Arithmetic operators
	//------------------------------------------------------
	//negative value
	T operator-(){
		T t;
		getLock();
		t = -val;
		unlock();
		return t;
	}

	//addition
	template <class T2>
	T operator+(T2 t2){
		T t;
		getLock();
		t =  val + t2;
		unlock();
		return t;
	}

	//subtraction
	template <class T2>
	T operator-(T2 t2){
		T t;
		getLock();
		t = val - t2;
		unlock();
		return t;
	}

	//mulipication
	template <class T2>
	T operator*(T2 t2){
		T t;
		getLock();
		t =  val * t2;
		unlock();
		return t;
	}

	//division
	template <class T2>
	T operator/(T2 t2){
		T t;
		getLock();
		t = val / t2;
		unlock();
		return t;
	}
	
	//modulus
	template <class T2>
	T operator%(T2 t2){
		T t;
		getLock();
		t = val % t2;
		unlock();
		return t;
	}

	//------------------------------------------------------

	

	
	//comparision operators
	//------------------------------------------------------
	template <class T2>
	bool operator==(T2 t2){
		bool flag;
		getLock();
		flag = val == t2;
		unlock();
		return flag;
	}

	template <class T2>
	bool operator!=(T2 t2){
		bool flag;
		getLock();
		flag = val != t2;
		unlock();
		return flag;
	}

	template <class T2>
	bool operator>(T2 t2){
		bool flag;
		getLock();
		flag = val > t2;
		unlock();
		return flag;
	}

	template <class T2>
	bool operator<(T2 t2){
		bool flag;
		getLock();
		flag = val < t2;
		unlock();
		return flag;
	}

	template <class T2>
	bool operator>=(T2 t2){
		bool flag;
		getLock();
		flag = val >= t2;
		unlock();
		return flag;
	}

	template <class T2>
	bool operator<=(T2 t2){
		bool flag;
		getLock();
		flag = val <= t2;
		unlock();
		return flag;
	}
	//------------------------------------------------------

private:
	//value been wrapped
	T val;

	//lock
	std::atomic_flag lock;

	//try acquire lock
	void getLock(){
		while (lock.test_and_set(std::memory_order_seq_cst));
	}

	//release lock
	void unlock(){
		lock.clear(std::memory_order_seq_cst);
	}
};

#endif