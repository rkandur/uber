#ifndef __DEPENDENCY_PLANNER_UTILS__
#define __DEPENDENCY_PLANNER_UTILS__

#include <iostream>
#include "Task.cxx"
#include <sstream>
#include <string>
#include <boost/regex.hpp>
#include <vector>
#include <fstream>
#include <iterator>
#include <boost/algorithm/string.hpp>
#include <unordered_map>
#include <unordered_set>
//TODO: remove unwanted headers

using namespace std;

class DependencyPlannerUtils {

	public:

		static std::unordered_map<std::string, Task> initialize_tasks(std::string &input_file) {

			std::unordered_map<std::string, Task> all_tasks;
			std::string line;
			ifstream input_file_handle (input_file);

			if (input_file_handle.is_open()) {

				while (getline(input_file_handle, line)) {
					//TODO: make this a constant.
					// Extract the structure of the input, this will be used to create the input tasks. 
					boost::regex reg_exp ("([a-z|A-Z|_]*)[[:space:]](\\()[[:space:]]([0-9]*)[[:space:]]([a-z|A-Z]*)[[:space:]](\\))[[:space:]](\\:)[[:space:]](.*)");
					std::vector<std::string> task_components;
					boost::regex_split(std::back_inserter(task_components), line, reg_exp);

					// Make sure we have all the expected components in the current entry.
					if(task_components.size() != 6) {
						cout << "Could not parse the line : " << line << endl;
						continue;
					}

					// Extract the set of dependencies of the current task.
					std::string delimiters(",");
					std::vector<std::string> dependents;
					boost::split(dependents, task_components[5], boost::is_any_of(delimiters));

					// Construct the task object.
					Task t = new Task(task_components[0], atoi(task_components[2].c_str()), dependents);
					all_tasks.insert(std::make_pair<std::string, Task>(task_components[0], t));
				}

				input_file_handle.close();
			}

			return all_tasks;

		}

		static void add_parent_tasks(std::unordered_map<std::string, Task> &all_tasks) {

			// Add the parent tasks along with the dependent tasks.
			for(std::unordered_map<std::string, Task>::iterator it = all_tasks.begin(); it != all_tasks.end(); ++it) {
				for(std::vector<Task>::iterator dep_it = it->second.get_dependents().begin(); 
						dep_it != it->second.get_dependents().end(); ++dep_it) {
					(*dep_it).add_parent(it->second);
				}
			}

		}

		static void compute_total_times(std::unordered_map<std::string, Task> &all_tasks) {

			// Compute the maximum time that could be taken from the current task until all the tasks get executed.
			std::unordered_map<std::string, bool> visited;
			for(std::unordered_map<std::string, Task>::iterator it = all_tasks.begin(); it != all_tasks.end(); ++it) {
				visited.insert(std::make_pair<std::string, bool>(it->first, false));
			}

			for(std::unordered_map<std::string, Task>::iterator it = all_tasks.begin(); it != all_tasks.end(); ++it) {
				compute_total_time_util(visited, it->second);
			}

		}

		//Method which gets the tasks which have no dependents. The nextset of tasks to be executed
		// will be eventually learned.
		static std::list<Task> get_ready_to_start_tasks(std::unordered_map<std::string, Task> &all_tasks) {
			// These are the tasks that have no dependents. Which act as our starting point.
			std::list<Task> chosen_tasks;
			for (std::unordered_map<std::string, Task>::iterator it = all_tasks.begin(); it != all_tasks.end(); ++it) {
				if(it->second.get_dependents().empty()) {
					chosen_tasks.push_back(it->second);
				}
			}
			return chosen_tasks;
		}

		static bool is_ready_to_execute(Task &task, std::unordered_set<std::string> &executed_tasks) {
			for(std::vector<Task>::iterator it = task.get_dependents().begin(); it != task.get_dependents().end(); ++it) {
				if(executed_tasks.find((*it).get_task_id()) == executed_tasks.end()) {
					return false;
				}
			}
			return true;
		}

	private:

		static void compute_total_time_util(std::unordered_map<std::string, bool> &visited, Task &task) {

			// Mark the current task as visited.
			std::unordered_map<std::string, bool>::iterator got = visited.find(task.get_task_id());
			if(got != visited.end()) {
				got->second = true;
			}

			for(std::vector<Task>::iterator it = task.get_parents().begin(); it != task.get_parents().end(); ++it) {
				std::unordered_map<std::string, bool>::iterator vis_it = visited.find((*it).get_task_id());
				if(vis_it != visited.end() && vis_it->second != true) {
					compute_total_time_util(visited, *it);
				}
			}

			int maximum = 0;
			for(std::vector<Task>::iterator it = task.get_parents().begin(); it != task.get_parents().end(); ++it) {
				if(maximum < (*it).get_total_time_for_execution()) {
					maximum = (*it).get_total_time_for_execution();
				}
			}
			task.set_total_time_for_execution(task.get_total_time_for_execution()+maximum);

		}

};

#endif
