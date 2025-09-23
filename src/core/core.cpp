//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#include <string>
#include <fstream>
#include <sstream>
#include <filesystem>

#include "logging/logging.hpp"

#include "core/core.hpp"

using KalaHeaders::Log;
using KalaHeaders::LogType;

using std::string;
using std::ifstream;
using std::ostringstream;
using std::filesystem::exists;
using std::filesystem::path;
using std::filesystem::current_path;
using std::filesystem::copy;
using std::filesystem::copy_file;
using std::filesystem::is_directory;
using std::filesystem::is_regular_file;

namespace MoveProject
{
	bool Core::CheckPaths()
	{
		string currentPathStem = current_path().stem().string();
		Log::Print(
			"Currently located at parent folder '" + currentPathStem + "'.",
			"INIT",
			LogType::LOG_INFO);

		if (currentPathStem != "MoveProject")
		{
			Log::Print(
				"Incorrect parent folder detected! Executable must be located inside 'MoveProject' folder.",
				"INIT",
				LogType::LOG_ERROR);

			return false;
		}

		Log::Print("check paths");

		return true;
	}

	void Core::CopyFiles()
	{
		Log::Print("copy files");
	}
}