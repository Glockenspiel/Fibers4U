# Fibers
An alternative implmentation of co-routines with use of concurrent data structures in C++11

## How it works
- Create a task you want to run in parallel
- Construct the scheduler giving it the inital task
- Give the scheduler more tasks or have tasks create new tasks
- Keep track of the order of execution with counters
- Close threads and Deconstruct

Simple example:
```c++
#include "include/fbr.h"
using namespace fbr;
using namespace std;

int main(){
  //assume we have a class named Person
  Person *a = new Person();
  
  //create a task suing a binded function which takes a string argument
  Task<string> *task = new Task<string>(std::bind(&Person:setName, a));
  string name = "John";
  taskArg->setArgs(name);
  
  //construct the scheduler to use 4 threads
  //it must also pass the first task to run
  Scheduler *scheduler = new Scheduler(0,4, task,true,true);
	if (scheduler->getIsConstructed() == false){
		return 0;
	}
  
  //close the threads and clear memory
  scheduler->close();
  delete scheduler;
  delete a;
  delete task;
}
```

Usually you will want to print to the console asynchronously so there is also concurrent input and output
```c++
//asynchronous output
fbr::con_cout << "Hello ";
fbr::con_cout << "World!" << fbr::endl;

//asynchronous input
std::string s;
fbr::con_input(s, "Enter some text:");
fbr::con_cout << s << fbr::endl;
//notify the scheduler about the end of the input event
Scheduler::wakeUpMain(); 
```


## How the scheduler works
- When constructed, the scheduler will create several worker threads which will be ready to execute tasks
- The scheduler will assign a the starting task to one of the threads
- Any task can create new tasks and tasks are __not__ stored in a child parent relationship
- When a new task is created the task can either be ready to run right now or it is waiting on other tasks to complete. The task will either be added:
  - to the waiting list or 
  - to the priority queue (higher prioirty tasks are favoured)
- You can think of this as a similar but simple structure to how the operating system schedules processes. If you wish you can even write your own implementation of a scheduler and take advantage of the classes and concurrent data structures.
![Fiber](https://i.imgur.com/TqY5FSD.jpg)

## Counters
Counters are used to decide the order at which functions should be executed. They might seem overly simple but are very effective and easy to use.

- Create a counter
- Run a single task or a group of tasks on the scheduler
- This task or task group will have a set counter matching the number of tasks left to be completed
- Once all the tasks are completed the counter will have reached zero

```c++
Counter a("taskGroupName");
Counter b("ending");

//run multiple tasks async with high priority using the counter a
Scheduler::runTasks({ myTask1, myTask2, myTask3 }, Priority::high, &a);

//the counter will be set to the number of tasks in this group
//and each time one of these task are completed the counter
//will decrement the counter

//wait for counter "a" to reach zero
Scheduler::waitForCounter(&a, 0, myNextTask, &b);
```

## Concurrent data structures
This library offers a few useful concurrent data strucutres for primitive types and linked lists which are thread safe to read and write to. These data structures have all the same operator functionality as the C++11 standrard library. Some of the linked lists have slightly different functions based on async or sync access. However these structures are using atomic flags with spin locks. Here is a list of the types:
- int, float double, string, bool
- con_vector, con_queue, con_stack
- Counter

```c++
//primitive types
fbr::concurrent<int> a = 10;
a++; //all operators are supported
fbr::concurrent<bool> b = false;

//populating a concurrent vector
fbr::con_vector<int> mylist;
mylist.get_lock_extern();
  for (unsigned int i = 0; mylist.size_unsync() < 10; i++){
    mylist.push_back_unsync(i);
  }
mylist.unlock_extern();
```



