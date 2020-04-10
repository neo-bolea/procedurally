#pragma once

#include <filesystem>
#include <chrono>
#include <thread>
#include <unordered_map>
#include <string>
#include <functional>

// Define available file changes
enum class FileStatus 
{
	Created, Modified, Erased 
};

class FileWatcher
{
public:
	// Time interval at which we check the base folder for changes
	std::chrono::duration<int, std::milli> delay;

	// Keep a record of files from the base directory and their last modification time
	FileWatcher(std::string path_to_watch, std::chrono::duration<int, std::milli> delay) : path_to_watch{ path_to_watch }, delay{ delay } {
		for (auto &file : std::filesystem::recursive_directory_iterator(path_to_watch))
		{
			Paths[file.path().string()] = std::filesystem::last_write_time(file);
		}
	}

	// Monitor "path_to_watch" for changes and in case of a change execute the user supplied "action" function
	void Watch(const std::function<void(std::string, FileStatus)> &action) 
	{
		for(auto it = Paths.begin(); it != Paths.end();)
		{
			if (!std::filesystem::exists(it->first)) 
			{
				action(it->first, FileStatus::Erased);
				it = Paths.erase(it);
			}
			else { it++; }
		}

		// Check if a file was Created or Modified
		for (auto &file : std::filesystem::recursive_directory_iterator(path_to_watch))
		{
			auto current_file_last_write_time = std::filesystem::last_write_time(file);
			std::string path = file.path().string();

			if (!contains(path))
			{
				// File creation
				Paths[path] = current_file_last_write_time;
				action(path, FileStatus::Created);
			}
			else 
			{
				// File modification
				if (Paths[path] != current_file_last_write_time) 
				{
					Paths[path] = current_file_last_write_time;
					action(path, FileStatus::Modified);
				}
			}
		}
	}
private:
	bool contains(const std::string &key)
	{
		auto el = Paths.find(key);
		return el != Paths.end();
	}

	std::unordered_map<std::string, std::filesystem::file_time_type> Paths;
	std::string path_to_watch;
};