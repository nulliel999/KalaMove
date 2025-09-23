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
using std::string_view;
using std::ifstream;
using std::ostringstream;
using std::filesystem::exists;
using std::filesystem::path;
using std::filesystem::current_path;
using std::filesystem::copy;
using std::filesystem::copy_file;
using std::filesystem::is_directory;
using std::filesystem::is_regular_file;

constexpr string_view ROOT_NAME = "MoveProject";
constexpr string_view KW_NAME = "KalaWindow";

static const path PROJECT_ROOT_FIRST  = current_path();
static const path PROJECT_ROOT_SECOND = current_path().parent_path();
static const path PROJECT_ROOT_THIRD  = current_path().parent_path().parent_path();
static const path TARGETS_FILE = current_path() / "targets.txt";

static path TRUE_ROOT{};
static path KW_ROOT{};

static bool RootIsValid();
static void CopyToPath(path targetPath);

namespace MoveProject
{
	void Core::Run()
	{
		if (!RootIsValid()) return;

		return;
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
			"INIT",
			LogType::LOG_ERROR);

		return false;
	}

	if (!exists(TARGETS_FILE))
	{
		Log::Print(
			"Failed to find targets file from '" + TARGETS_FILE.string() + "'! It must be located in the same folder where the executable is placed at.",
			"INIT",
			LogType::LOG_ERROR);

		return false;
	}

	Log::Print(
		"Found true MoveProject root from '" + TRUE_ROOT.string() + "'.",
		"INIT",
		LogType::LOG_SUCCESS);

	KW_ROOT = TRUE_ROOT.parent_path() / KW_NAME;

	if (!exists(KW_ROOT))
	{
		Log::Print(
			"Failed to find KalaWindow root folder from '" + KW_ROOT.string() + "'!",
			"INIT",
			LogType::LOG_ERROR);

		return false;
	}

	Log::Print(
		"Found KalaWindow root folder from '" + KW_ROOT.string() + "'.",
		"INIT",
		LogType::LOG_SUCCESS);

	return true;
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

	Log::Print(
		"Starting to copy to path '" + targetPath.string() + "'.",
		"COPY_FILE",
		LogType::LOG_INFO);

	//handle copy here...

	Log::Print(
		"Finished copying content to '" + targetPath.string() + "'!",
		"COPY_FILE",
		LogType::LOG_SUCCESS);
}