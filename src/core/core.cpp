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

static const path PROJECT_ROOT = current_path();
static const path KW_ROOT = current_path().parent_path() / "KalaWindow";

static const path ELYPSO_ENGINE_ROOT = current_path().parent_path() / "Elypso-engine";
static const path ELYPSO_HUB_ROOT    = current_path().parent_path() / "Elypso-hub";
static const path CIRCUIT_CHAN_ROOT  = current_path().parent_path() / "Circuit-chan";
static const path HINNAMASIN_ROOT    = current_path().parent_path() / "Hinnamasin";

static bool RootIsValid();
static void CopyToPath(path targetPath);

namespace MoveProject
{
	void Core::Run()
	{
		if (!RootIsValid()) return;

		CopyToPath(ELYPSO_ENGINE_ROOT);
		CopyToPath(ELYPSO_HUB_ROOT);
		CopyToPath(CIRCUIT_CHAN_ROOT);
		CopyToPath(HINNAMASIN_ROOT);

		return;
	}
}

bool RootIsValid()
{
	string currentPathStem = PROJECT_ROOT.stem().string();

	if (PROJECT_ROOT.stem().string() != "MoveProject")
	{
		Log::Print(
			"Incorrect parent folder detected! Executable must be located inside 'MoveProject' folder.",
			"INIT",
			LogType::LOG_ERROR);

		return false;
	}

	Log::Print(
		"Currently located at folder '" + PROJECT_ROOT.string() + "'.",
		"INIT",
		LogType::LOG_SUCCESS);

	if (!exists(KW_ROOT))
	{
		Log::Print(
			"Failed to find KalaWindow root folder!",
			"INIT",
			LogType::LOG_ERROR);

		return false;
	}

	Log::Print(
		"Found KalaWindow root folder at '" + KW_ROOT.string() + "'.",
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