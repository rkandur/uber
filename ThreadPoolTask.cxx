#include "ThreadPoolTask.h"

ThreadPoolTask::ThreadPoolTask(int time, Task &task) {
	id_ = ++thread_count_;
	time_ = time;
	task_ = task;
}

int ThreadPoolTask::thread_count_ = 0;
