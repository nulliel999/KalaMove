//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <array>
#include <filesystem>

#include "core.hpp"
#include "external/logging.hpp"
#include "external/string_helpers.hpp"

using KalaHeaders::Log;
using KalaHeaders::LogType;
using KalaHeaders::StartsWith;
using KalaHeaders::SplitString;
using KalaHeaders::RemoveAllFromString;
using KalaHeaders::ReplaceAllFromString;

using std::string;
using std::to_string;
using std::string_view;
using std::ifstream;
using std::getline;
using std::ostringstream;
using std::vector;
using std::array;
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

//Each kalamovefile block in the .kmf file
struct KMF
{
	int line{};
	path origin{};
	vector<path> targets{};
	string action{};
};

static const path thisPath = current_path();

constexpr string_view KMF_VERSION_NUMBER = "1.0";
constexpr string_view KMF_VERSION_NAME = "#KMF VERSION 1.0";
constexpr string_view KMF_EXTENSION = ".ktf";

static const array<string, 3> actionTypes =
{
	"move",    //move file to first target and overwrite, forcecopy to all other targets
	"copy",    //copy file to target only if target does not already have this file
	"fullcopy" //copy file to target and overwrite
};

static vector<path> GetAllKMFFiles();
static vector<KMF> GetAllKMFContent(path kmfFile);
static void HandleKMFBlock(KMF kmfBlock);

static void Exit()
{
	Log::Print("\nPress 'Enter' to exit...");
	cin.get();
	quick_exit(0);
}

namespace KalaMove
{
	void Core::Run()
	{
		ostringstream oss{};

		oss << "==============================\n"
			<< "== KALAMOVE 1.0\n"
			<< "==============================\n";

		Log::Print(oss.str());

		vector<path> kmfFiles = GetAllKMFFiles();

		if (kmfFiles.empty())
		{
			Log::Print(
				"Did not find any .kmf files. There is nothing to copy.",
				"GET_KMF",
				LogType::LOG_ERROR);

			Exit();
		}

		vector<KMF> kmfContent{};
		for (const auto& kmfFile : kmfFiles)
		{
			vector<KMF> thisKmfContent = GetAllKMFContent(kmfFile);

			kmfContent.insert(
				kmfContent.end(), 
				thisKmfContent.begin(), 
				thisKmfContent.end());
		}

		if (kmfContent.empty())
		{
			Log::Print(
				"Did not find any valid kmf content. There is nothing to copy.",
				"READ_KMF",
				LogType::LOG_ERROR);

			Exit();
		}

		for (const auto& kmf : kmfContent)
		{
			HandleKMFBlock(kmf);
		}

		Exit();
	}
}

vector<path> GetAllKMFFiles()
{
	vector<path> result{};

	for (const auto& file : directory_iterator(current_path()))
	{
		//only allow kmf files
		if (is_regular_file(path(file))
			&& path(file).extension() == ".kmf")
		{
			Log::Print(
				"Found valid .kmf file '" + path(file).filename().string() + "'!",
				"GET_KMF",
				LogType::LOG_SUCCESS);

			result.push_back(path(file));
		}
	}

	return result;
}

vector<KMF> GetAllKMFContent(path kmfFile)
{
	vector<KMF> result{};

	bool foundVersion = false;
	string line{};
	static int lineNumber{};

	static bool hasOrigin{};
	static bool hasTargets{};
	static bool hasAction{};

	static struct KMF kmfBlock {};

	ifstream file(kmfFile);

	if (!file.is_open())
	{
		Log::Print(
			"Failed to read kmf file '" + kmfFile.string() + "'!",
			"READ_KMF",
			LogType::LOG_ERROR);

		return{};
	}

	auto IsValidVersion = [](
		const string& line,
		const string& kmfPath,
		int lineNumber)
		{
			//correct version was found
			if (line == KMF_VERSION_NAME) return true;

			//found version tag but incorrect version or invalid version string was found
			if (!StartsWith(line, "#KMF VERSION "))
			{
				Log::Print(
					"Kmf file '" + kmfPath + "' has an invalid version '" + line + "' at line '" + to_string(lineNumber) + "'!",
					"READ_KMF",
					LogType::LOG_ERROR);

				return false;
			}
			else
			{
				vector<string> split = SplitString(line, " ");

				if (split.size() > 3)
				{
					Log::Print(
						"Kmf file '" + kmfPath + "' has an invalid version '" + line + "' with too many values at line '" + to_string(lineNumber) + "'!",
						"READ_KMF",
						LogType::LOG_ERROR);

					return false;
				}

				if (split[2] != KMF_VERSION_NUMBER)
				{
					Log::Print(
						"Kmf file '" + kmfPath + "' has an invalid version number '" + split[2] + "' at line '" + to_string(lineNumber) + "'!",
						"READ_KMF",
						LogType::LOG_ERROR);

					return false;
				}
			}

			return true;
		};

	auto StoreContent = [&]()
		{
			result.push_back(kmfBlock);

			kmfBlock.origin.clear();
			kmfBlock.targets.clear();
			kmfBlock.action.clear();

			hasOrigin = false;
			hasTargets = false;
			hasAction = false;
		};

	while (getline(file, line))
	{
		bool isLastLine = file.peek() == EOF;

		lineNumber++;
		
		//skip empty and comment lines
		if (line.empty()
			|| StartsWith(line, "//"))
		{
			continue;
		}

		//version must always be at the top
		if (!foundVersion)
		{
			foundVersion = IsValidVersion(
				line,
				kmfFile.string(),
				lineNumber);

			if (isLastLine) StoreContent();

			if (!foundVersion) return{};

			Log::Print(
				"Kmf file '" + kmfFile.stem().string() + "' has a correct version '" + line + "' at line '" + to_string(lineNumber) + "'.",
				"READ_KMF",
				LogType::LOG_DEBUG);

			continue;
		}

		//
		// GET ORIGIN PATH
		//

		if (!hasOrigin)
		{
			kmfBlock.line = lineNumber;

			if (!StartsWith(line, "origin: "))
			{
				Log::Print(
					"Kmf file '" + kmfFile.string() + "' has a missing origin key at line '" + to_string(lineNumber) + "'!",
					"READ_KMF",
					LogType::LOG_ERROR);

				if (isLastLine) StoreContent();
				return {};
			}

			string originPathString = RemoveAllFromString(line, "origin: ");
			if (originPathString.empty())
			{
				Log::Print(
					"Kmf file '" + kmfFile.string() + "' has an origin path with no assigned value at line '" + to_string(lineNumber) + "'!",
					"READ_KMF",
					LogType::LOG_ERROR);

				if (isLastLine) StoreContent();
				return {};
			}

			bool isAbsolute = StartsWith(originPathString, "@@");

			//add path relative to exe as prefix if not absolute
			if (!isAbsolute)
			{
#ifdef _WIN32
				originPathString = ReplaceAllFromString(originPathString, "@", "\\");
#else
				originPathString = ReplaceAllFromString(originPathString, "@", "/");
#endif
			}

			path originPath = thisPath / originPathString;
			if (!exists(originPath))
			{
				Log::Print(
					"Kmf file '" + kmfFile.string() + "' has an origin path '" + originPathString + "' at line '" + to_string(lineNumber) + "' that does not exist!",
					"READ_KMF",
					LogType::LOG_ERROR);

				if (isLastLine) StoreContent();
				return {};
			}

			Log::Print(
				"Kmf file '" + kmfFile.stem().string() + "' has a correct origin '" + originPathString + "' at line '" + to_string(lineNumber) + "'.",
				"READ_KMF",
				LogType::LOG_DEBUG);

			kmfBlock.origin = originPath;
			hasOrigin = true;

			if (isLastLine) StoreContent();
			continue;
		}

		//
		// GET TARGET PATHS
		//

		if (!hasTargets)
		{
			if (!StartsWith(line, "target: "))
			{
				Log::Print(
					"Kmf file '" + kmfFile.string() + "' has a missing target key at line '" + to_string(lineNumber) + "'!",
					"READ_KMF",
					LogType::LOG_ERROR);

				if (isLastLine) StoreContent();
				return {};
			}

			string targetPathsString = RemoveAllFromString(line, "target: ");
			if (targetPathsString.empty())
			{
				Log::Print(
					"Kmf file '" + kmfFile.string() + "' has no assigned target path values at line '" + to_string(lineNumber) + "'!",
					"READ_KMF",
					LogType::LOG_ERROR);

				if (isLastLine) StoreContent();
				return {};
			}

			vector<string> targets = SplitString(targetPathsString, ", ");
			if (targets.empty())
			{
				Log::Print(
					"Kmf file '" + kmfFile.string() + "' has no assigned target path values at line '" + to_string(lineNumber) + "'!",
					"READ_KMF",
					LogType::LOG_ERROR);

				if (isLastLine) StoreContent();
				return {};
			}

			for (const auto& target : targets)
			{
				string correctTarget = target;

				bool isAbsolute = StartsWith(correctTarget, "@@");

				//add path relative to exe as prefix if not absolute
				if (!isAbsolute)
				{
#ifdef _WIN32
					correctTarget = ReplaceAllFromString(correctTarget, "@", "\\");
#else
					correctTarget = ReplaceAllFromString(correctTarget, "@", "/");
#endif
				}

				path fullTarget = thisPath / correctTarget;
				if (kmfBlock.origin == fullTarget)
				{
					Log::Print(
						"Kmf file '" + kmfFile.string() + "' has an invalid target at line '" + to_string(lineNumber) + "', it was skipped. Origin cannot be the same path as target path '" + correctTarget + "'!",
						"READ_KMF",
						LogType::LOG_WARNING);

					continue;
				}

				if (kmfBlock.origin.has_extension()
					&& !fullTarget.has_extension())
				{
					Log::Print(
						"Kmf file '" + kmfFile.string() + "' has an invalid target at line '" + to_string(lineNumber) + "', it was skipped. Origin has an extension but target '" + correctTarget + "' does not!",
						"READ_KMF",
						LogType::LOG_WARNING);

					continue;
				}

				if (!kmfBlock.origin.has_extension()
					&& fullTarget.has_extension())
				{
					Log::Print(
						"Kmf file '" + kmfFile.string() + "' has an invalid target at line '" + to_string(lineNumber) + "', it was skipped. Origin does not have an extension but target '" + correctTarget + "' does!",
						"READ_KMF",
						LogType::LOG_WARNING);

					continue;
				}

				if (kmfBlock.origin.has_extension()
					&& fullTarget.has_extension()
					&& kmfBlock.origin.extension().string()
					!= fullTarget.extension().string())
				{
					Log::Print(
						"Kmf file '" + kmfFile.string() + "' has an invalid target at line '" + to_string(lineNumber) + "', it was skipped. Origin and target are regular files, origin has extension '" + kmfBlock.origin.extension().string() + "' but target '" + correctTarget + "' did not have the same extension!",
						"READ_KMF",
						LogType::LOG_WARNING);

					continue;
				}

				Log::Print(
					"Kmf file '" + kmfFile.stem().string() + "' has correct targets at line '" + to_string(lineNumber) + "'.",
					"READ_KMF",
					LogType::LOG_DEBUG);

				kmfBlock.targets.push_back(fullTarget);
			}

			//also ensure any values actually were moved to kmf block if invalid paths were skipped
			if (kmfBlock.targets.empty())
			{
				Log::Print(
					"Kmf file '" + kmfFile.string() + "' has no assigned target path values at line '" + to_string(lineNumber) + "'!",
					"READ_KMF",
					LogType::LOG_ERROR);

				if (isLastLine) StoreContent();
				return {};
			}

			hasTargets = true;

			if (isLastLine) StoreContent();
			continue;
		}

		//
		// GET ACTION STATE
		//

		if (!hasAction)
		{
			if (!StartsWith(line, "action: "))
			{
				Log::Print(
					"Kmf file '" + kmfFile.string() + "' has a missing action key at line '" + to_string(lineNumber) + "'!",
					"READ_KMF",
					LogType::LOG_ERROR);

				if (isLastLine) StoreContent();
				return {};
			}

			string actionString = RemoveAllFromString(line, "action: ");
			if (actionString.empty())
			{
				Log::Print(
					"Kmf file '" + kmfFile.string() + "' has no assigned action value at line '" + to_string(lineNumber) + "'!",
					"READ_KMF",
					LogType::LOG_ERROR);

				if (isLastLine) StoreContent();
				return {};
			}

			if (find(actionTypes.begin(), actionTypes.end(), actionString) == actionTypes.end())
			{
				Log::Print(
					"Kmf file '" + kmfFile.string() + "' has an invalid action value '" + actionString + "' at line '" + to_string(lineNumber) + "'!",
					"READ_KMF",
					LogType::LOG_ERROR);

				if (isLastLine) StoreContent();
				return {};
			}

			kmfBlock.action = actionString;
			hasAction = true;

			StoreContent();

			Log::Print(
				"Kmf file '" + kmfFile.stem().string() + "' has a correct action '" + actionString + "' at line '" + to_string(lineNumber) + "'.",
				"READ_KMF",
				LogType::LOG_DEBUG);
		}
	}

	if (!foundVersion)
	{
		Log::Print(
			"Kmf file '" + kmfFile.string() + "' does not have a version!",
			"READ_KMF",
			LogType::LOG_ERROR);

		return{};
	}

	for (const auto& kmf : result)
	{
		if (kmf.targets.empty())
		{
			string targetLine = to_string(kmf.line + 1);

			Log::Print(
				"Kmf file '" + kmfFile.string() + "' is has no assigned valid targets at line '" + targetLine + "'!",
				"READ_KMF",
				LogType::LOG_ERROR);

			return{};
		}

		if (kmf.action.empty())
		{
			string targetLine = to_string(kmf.line + 2);

			Log::Print(
				"Kmf file '" + kmfFile.string() + "' is has no assigned action at line '" + targetLine + "'!",
				"READ_KMF",
				LogType::LOG_ERROR);

			return{};
		}
	}

	return result;
}

void HandleKMFBlock(KMF kmfBlock)
{
	for (const auto& target : kmfBlock.targets)
	{
		Log::Print(
			"Starting to " + kmfBlock.action + " origin '" + kmfBlock.origin.string() + "' to target '" + target.string() + "'.",
			"MOVE_KMF",
			LogType::LOG_SUCCESS);

		if (kmfBlock.action == "move")
		{

		}
		else if (kmfBlock.action == "copy")
		{

		}
		else if (kmfBlock.action == "forcecopy")
		{

		}
	}
}