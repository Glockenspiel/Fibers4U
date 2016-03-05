#ifndef CON_SOA_VECTOR_H
#define CON_SOA_VECTOR_H

#include <tuple>
#include <vector>

using std::vector;
using std::tuple;

template<class A, class B>
class con_soa_vector{
public:
	con_soa_vector(){}

	A& getA(int index){
		return aList[index];
	}

	B& getB(int index){
		return bList[index];
	}

	void add(A a, B b){
		aList.push_back(a);
		bList.push_back(b);
	}

	int size(){
		return aList.size();
	}

private:
	vector<A> aList;
	vector<B> bList;
};



#endif