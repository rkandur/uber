#ifndef __DEPENDENCY_PLANNER__
#define __DEPENDENCY_PLANNER__

#include <queue>
#include <unordered_set>
#include "DependencyPlannerUtils.cxx"
#include "ThreadPoolTask.cxx"

struct thread_pool_min_heap_comparator {
	bool operator()(ThreadPoolTask i, ThreadPoolTask j) {
		return i.get_time() > j.get_time();
	}
};

struct total_time_max_heap_comparator {
	bool operator()(Task i, Task j) {
		return i.get_total_time_for_execution() < j.get_total_time_for_execution();
	}
};

int main(int argc, char *argv[]) {

	std::string input_file = argv[0];
	int max_parallelism = atoi(argv[1].c_str());

	// Initialize all the task information from input file.
	std::unordered_map<std::string, Task> all_tasks = DependencyPlannerUtils::initialize_tasks(input_file);
	// Add an additional item called Parent Tasks which will be used to schedule the next task at any given instance. 
	DependencyPlannerUtils::add_parent_tasks(all_tasks);
	// Add an additional computed time value which will be used to schedule the next task at any given instance. 
	DependencyPlannerUtils::compute_total_times(all_tasks);

	// These are the tasks which do not have any dependents.
	std::list<Task> starting_tasks = DependencyPlannerUtils::get_ready_to_start_tasks(all_tasks);

	// Queue which has the tasks that are ready to be executed.
	priority_queue<Task, std::vector<Task>, total_time_max_heap_comparator> ready_to_be_executed;
	while(starting_tasks.size()) {
		ready_to_be_executed.push(starting_tasks.front());
		starting_tasks.pop_front();
	}

	// Container which holds the set of tasks that are executed.
	std::unordered_set<std::string> executed_tasks;

	// Variables which are used to track if all the input tasks are executed or not.
	int total_number_tasks = all_tasks.size();

	// Priority queue which will simulate the multi threading for us.
	priority_queue<ThreadPoolTask, std::vector<ThreadPoolTask>, thread_pool_min_heap_comparator> thread_pool;

	// Variable which is used to denote the current time line to simulate multi-threading.
	int timeline = 0;

	while(executed_tasks.size() < total_number_tasks) {

		// If the max parallelism is not reached and there are elements ready to be executed.
		while(thread_pool.size() <= max_parallelism && ready_to_be_executed.size() > 0) {
			// TODO: How to modify this so that we have fixed Actor numbers?
			cout << ready_to_be_executed.top().get_task_id() << "( assigned to Actor #) " << endl;
			thread_pool.push(new ThreadPoolTask(
						timeline+ready_to_be_executed.top().get_time_to_complete(),
						(Task)ready_to_be_executed.top()));
			ready_to_be_executed.pop();
		}

		// See if any of the tasks have finished executing.
		priority_queue<ThreadPoolTask, std::vector<ThreadPoolTask>, thread_pool_min_heap_comparator> temp_queue;
		while(thread_pool.size() > 0 && thread_pool.top().get_time() >= timeline) {
			temp_queue.push(thread_pool.top());
			thread_pool.pop();
		}

		// While marking the tasks as executed, push the tasks whose dependents 
		// are all executed to a queue which is used to pick up new tasks for executing.
		while(temp_queue.size() > 0) {
			Task t = temp_queue.top().get_task();
			executed_tasks.insert(t.get_task_id());
			for(std::vector<Task>::iterator it = t.get_parents().begin(); it != t.get_parents().end(); ++it) {
				if(DependencyPlannerUtils::is_ready_to_execute(*it, executed_tasks)) {
					ready_to_be_executed.push(*it);
				}
			}
			temp_queue.pop();
		}

		// Time keeps increasing.
		// TODO: See how this can be avoided. Looks like the dependents are only considered from next cycle. 
		// So there is a one cycle gap.
		timeline += 1;

	}

}

#endif
