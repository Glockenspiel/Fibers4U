#ifndef BASE_TASK_H
#define BASE_TASK_H

namespace fbr{
	//this is the base class for all task types
	class BaseTask{
	public:
		//called when required to run the task
		virtual void run() = 0;
		bool isReusable();
		void setReuseable(bool reuse);

	private:
		bool reusable = false;
	};
}
#endif