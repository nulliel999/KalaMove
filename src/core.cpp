//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <filesystem>

#include "core.hpp"
#include "logging/logging.hpp"

using KalaHeaders::Log;
using KalaHeaders::LogType;

using std::string;
using std::string_view;
using std::ifstream;
using std::getline;
using std::ostringstream;
using std::vector;
using std::cin;
using std::filesystem::exists;
using std::filesystem::path;
using std::filesystem::current_path;
using std::filesystem::copy;
using std::filesystem::copy_file;
using std::filesystem::is_directory;
using std::filesystem::is_regular_file;
using std::filesystem::directory_iterator;
using std::filesystem::recursive_directory_iterator;

constexpr string_view ROOT_NAME = "MoveProject";
constexpr string_view KW_NAME = "KalaWindow";
constexpr string_view EXT_NAME = "_external_shared";

static const path PROJECT_ROOT_FIRST  = current_path();
static const path PROJECT_ROOT_SECOND = current_path().parent_path();
static const path PROJECT_ROOT_THIRD  = current_path().parent_path().parent_path();
static const path TARGETS_FILE = current_path() / "targets.txt";

static path TRUE_ROOT{};
static path KW_ROOT{};

static bool RootIsValid();
static vector<string> GetTargetRootPaths();
static void CopyToPath(path targetPath);

namespace MoveProject
{
	void Core::Run()
	{
		if (!RootIsValid()) return;

		vector<string> result = GetTargetRootPaths();

		for (const auto& value : result)
		{
			CopyToPath(TRUE_ROOT.parent_path() / value);
		}

		Log::Print("\nPress 'Enter' to exit...");
		cin.get();
	}
}

bool RootIsValid()
{
	string firstStem = PROJECT_ROOT_FIRST.stem().string();
	string secondStem = PROJECT_ROOT_SECOND.stem().string();
	string thirdStem = PROJECT_ROOT_THIRD.stem().string();

	bool foundRoot{};

	if (firstStem == ROOT_NAME)
	{
		foundRoot = true;
		TRUE_ROOT = PROJECT_ROOT_FIRST;
	}
	else if (secondStem == ROOT_NAME)
	{
		foundRoot = true;
		TRUE_ROOT = PROJECT_ROOT_SECOND;
	}
	else if (thirdStem == ROOT_NAME)
	{
		foundRoot = true;
		TRUE_ROOT = PROJECT_ROOT_THIRD;
	}

	if (!foundRoot)
	{
		Log::Print(
			"Executable is not located in the right folder! Place it inside 'MoveFolder' root or first or second subfolder.",
			"ROOT",
			LogType::LOG_ERROR);

		return false;
	}

	Log::Print(
		"Found true MoveProject root from '" + TRUE_ROOT.string() + "'.",
		"ROOT",
		LogType::LOG_SUCCESS);

	KW_ROOT = TRUE_ROOT.parent_path() / KW_NAME;

	if (!exists(KW_ROOT))
	{
		Log::Print(
			"Failed to find KalaWindow root folder from '" + KW_ROOT.string() + "'!",
			"ROOT",
			LogType::LOG_ERROR);

		return false;
	}

	Log::Print(
		"Found KalaWindow root folder from '" + KW_ROOT.string() + "'.",
		"ROOT",
		LogType::LOG_SUCCESS);

	return true;
}

vector<string> GetTargetRootPaths()
{
	if (!exists(TARGETS_FILE))
	{
		Log::Print(
			"Failed to find targets file from '" + TARGETS_FILE.string() + "'! It must be located in the same folder where the executable is placed at.",
			"TARGET_FILE",
			LogType::LOG_ERROR);

		return{};
	}

	Log::Print(
		"Found targets file '" + TARGETS_FILE.string() + "'! Starting to read from it.",
		"TARGET_FILE",
		LogType::LOG_SUCCESS);

	vector<string> result{};

	ifstream file(TARGETS_FILE);

	if (!file.is_open())
	{
		Log::Print(
			"Failed to open targets file '" + TARGETS_FILE.string() + "' to read it!",
			"TARGET_FILE",
			LogType::LOG_ERROR);

		return{};
	}

	string line{};
	while (getline(file, line))
	{
		result.push_back(line);
	}

	file.close();

	if (result.empty())
	{
		Log::Print(
			"Did not find any target paths from targets file, there is nothing to copy.",
			"TARGET_FILE",
			LogType::LOG_WARNING);

		return{};
	}

	return result;
}

void CopyToPath(path targetPath)
{
	if (!exists(targetPath))
	{
		Log::Print(
			"Target path '" + targetPath.string() + "' does not exist, skipping copy.",
			"COPY_FILE",
			LogType::LOG_WARNING);

		return;
	}
	if (!is_directory(targetPath))
	{
		Log::Print(
			"Target path '" + targetPath.string() + "' is not a directory, skipping copy.",
			"COPY_FILE",
			LogType::LOG_WARNING);

		return;
	}

	string targetStem = targetPath.stem().string();

	if (targetStem == ROOT_NAME
		|| targetStem == KW_NAME)
	{
		Log::Print(
			"Target path '" + targetPath.string() + "' cannot be 'KalaWindow' or 'MoveTarget'.",
			"COPY_FILE",
			LogType::LOG_WARNING);

		return;
	}

	path external_shared{};

	for (const auto& target : directory_iterator(targetPath))
	{
		if (!is_directory(target)) continue;

		if (path(target).stem() == EXT_NAME)
		{
			external_shared = target;
			break;
		}
	}

	if (external_shared.empty())
	{
		Log::Print(
			"External shared folder was not found in target path '" + targetPath.string() + "', skipping copy",
			"COPY_FILE",
			LogType::LOG_WARNING);

		return;
	}
	if (!exists(external_shared))
	{
		Log::Print(
			"External shared folder '" + external_shared.string() + "' does not exist, skipping copy",
			"COPY_FILE",
			LogType::LOG_WARNING);

		return;
	}

	Log::Print(
		"Target path '" + targetPath.stem().string() + "' is valid! Starting to copy to it.",
		"TARGET_FILE",
		LogType::LOG_SUCCESS);

	//handle copy here...

	Log::Print(
		"Finished copying content to '" + targetPath.stem().string() + "'!",
		"COPY_FILE",
		LogType::LOG_SUCCESS);
}