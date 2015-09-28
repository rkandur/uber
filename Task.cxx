#ifndef __H_TASK__
#define __H_TASK__

#include <vector>
#include <string>

using namespace std;

class Task {

	private:
		std::string task_id_;
		int time_to_complete_;
		int total_time_for_execution_;
		std::vector<Task> dependents_;
		std::vector<Task> parents_;

	public:

		Task(std::string &task_id, int time_to_complete, std::vector<Task> &dependents) {
			set_task_id(task_id);
			set_time_to_complete(time_to_complete);
			set_total_time_for_execution(time_to_complete);
			set_dependents(dependents);
		}

		void set_task_id(std::string &task_id) {
			task_id_ = task_id;
		}

		std::string get_task_id () const {
			return task_id_;
		}

		void set_time_to_complete(int time) {
			time_to_complete_ = time;
		}

		int get_time_to_complete() const {
			return time_to_complete_;
		}

		void set_total_time_for_execution(int time) {
			total_time_for_execution_ = time;
		}

		int get_total_time_for_execution() const {
			return time_to_complete_;
		}

		void set_dependents(std::vector<Task> &dependents) {
			//TODO: How to set a collection of set?
			dependents_ = dependents;
		}

		std::vector<Task> get_dependents() const {
			return dependents_;
		}

		void add_dependent(Task dependent) {
			dependents_.push_back(dependent);
		}

		void set_parents(std::vector<Task> &parents) {
			//TODO: How to set a collection of set?
			parents_ = parents;
		}

		std::vector<Task> get_parents() const {
			return parents_;
		}

		void add_parent(Task &parent) {
			parents_.push_back(parent);
		}

};

#endif
