#ifndef DEL_TASK_H
#define DEL_TASK_H

#include "include\BaseTask.h"
#include <tuple>


//sequence generator
//this finds the sequece of the parameters
namespace helper
{
	template <std::size_t... Params>
	struct index {};

	template <std::size_t N, std::size_t... Params>
	struct gen_seq : gen_seq<N - 1, N - 1, Params...> {};

	template <std::size_t... Params>
	struct gen_seq<0, Params...> : index<Params...>{};
}

template <typename... Params>
class DelTask : public BaseTask
{
private:
	std::function<void(Params...)> m_func;
	std::tuple<Params...> m_args;
public:
	//construct the task 
	DelTask(std::function<void(Params...)> func) : m_func(func)
	{}

	//set the arguments
	void set(Params... args)
	{
		m_args = std::make_tuple(std::forward<Params>(args)...);
	}

	//run the task
	void run()
	{
		func(m_args);
	}

private:
	//run the function with the given arguments and make use of the squence generator
	template <typename... Args, std::size_t... Is>
	void func(std::tuple<Args...>& tup, helper::index<Is...>)
	{
		m_func(std::get<Is>(tup)...);
	}

	//this is called on consturction to make use of the sequence generator
	template <typename... Args>
	void func(std::tuple<Args...>& tup)
	{
		func(tup, helper::gen_seq<sizeof...(Args)>{});
	}
};

#endif