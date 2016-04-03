#ifndef ELEMENT_H
#define ELEMENT_H

//an element structure used for containers such as con_vector, con_stack and con_queue

namespace fbr{
	template<class T>
	struct element{
		T val;
		element<T>* next;
	};
}

#endif