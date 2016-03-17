#ifndef ELEMENT_H
#define ELEMENT_H

namespace fbr{
	template<class T>
	struct element{
		T val;
		element<T>* next;
	};
}

#endif