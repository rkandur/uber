#ifndef __THREAD_POOL_TASK__
#define __THREAD_POOL_TASK__

#include "Task.cxx"

class ThreadPoolTask {

	public:

		ThreadPoolTask(int time, Task &task);

		int get_time() const {
			return time_;
		}

		Task get_task() const {
			return task_;
		}

		int get_thread_id() const {
			return id_;
		}

	private:
		static int thread_count_;
		Task task_;
		int time_;
		int id_;

};

#endif
