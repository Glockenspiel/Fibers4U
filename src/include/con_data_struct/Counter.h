#ifndef COUNTER_H
#define COUTNER_H

#include <atomic>
#include <string>

namespace fbr{
	//concurrent data structure for an integer
	class Counter{
	public:
		Counter(std::string name);

		//add value to counter
		void add(int a);

		//subtract value from counter
		void sub(int a);

		//get value from counter
		int get();

		//Operater overloading (++,--,+=,-=)
		Counter& operator++(int){
			add(1);
			return *this;
		}

		Counter& operator--(int){
			sub(1);
			return *this;
		}

		Counter& operator+=(int a){
			add(a);
			return *this;
		}

		Counter& operator-=(int a){
			sub(a);
			return *this;
		}

		std::string getName();

	private:
		int count=0;
		std::atomic_flag lock;
		int id;
		std::string name;
	};
}
#endif