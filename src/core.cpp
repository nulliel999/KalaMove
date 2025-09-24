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

namespace KalaMove
{
	void Core::Run()
	{
		vector<path> kmfFiles = GetAllKMFFiles();

		if (kmfFiles.empty())
		{
			Log::Print(
				"Did not find any .kmf files. There is nothing to copy.",
				"GET_KMF",
				LogType::LOG_ERROR);

			return;
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
				"Did not find any kmf content. There is nothing to copy.",
				"READ_KMF",
				LogType::LOG_ERROR);

			return;
		}

		for (const auto& kmf : kmfContent)
		{
			HandleKMFBlock(kmf);
		}

		Log::Print("\nPress 'Enter' to exit...");
		cin.get();
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
		path kmfFile)
		{
			//correct version was found
			if (line == KMF_VERSION_NAME)
			{
				return true;
			}

			//found version tag but incorrect version or invalid version string was found
			if (!StartsWith(line, "#KMF VERSION ")) return false;
			else
			{
				vector<string> split = SplitString(line, " ");
				if (split.size() != 3)
				{
					Log::Print(
						"Kmf file '" + kmfFile.string() + "' has invalid version '" + line + "'!",
						"READ_KMF",
						LogType::LOG_ERROR);

					return false;
				}
				else
				{
					if (split[2] != KMF_VERSION_NUMBER)
					{
						Log::Print(
							"Kmf file '" + kmfFile.string() + "' has invalid version '" + split[2] + "'!",
							"READ_KMF",
							LogType::LOG_ERROR);

						return false;
					}
				}
			}

			return true;
		};

	bool foundVersion = false;
	string line{};
	while (getline(file, line))
	{
		//skip empty and comment lines
		if (line.empty()
			|| StartsWith(line, "//"))
		{
			continue;
		}

		//version must always be at the top
		if (!foundVersion)
		{
			foundVersion = IsValidVersion(line, kmfFile);
			if (!foundVersion) return{};
		}

		//
		// START GATHERING KTF BLOCK DATA
		//

		static bool hasOrigin{};
		static bool hasTargets{};
		static bool hasAction{};

		static struct KMF kmfBlock{};

		if (!hasOrigin
			&& !hasTargets
			&& !hasAction)
		{
			//
			// GET ORIGIN PATH
			//

			if (!hasOrigin)
			{
				if (!StartsWith(line, "origin: "))
				{
					Log::Print(
						"Kmf file '" + kmfFile.string() + "' has a missing origin key!",
						"READ_KMF",
						LogType::LOG_ERROR);

					return {};
				}

				string originPathString = RemoveAllFromString(line, "origin: ");
				if (originPathString.empty())
				{
					Log::Print(
						"Kmf file '" + kmfFile.string() + "' has an origin path with no assigned value!",
						"READ_KMF",
						LogType::LOG_ERROR);

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
						"Kmf file '" + kmfFile.string() + "' has an origin path '" + originPathString + "' that does not exist!",
						"READ_KMF",
						LogType::LOG_ERROR);

					return {};
				}

				kmfBlock.origin = originPath;
				hasOrigin = true;
			}

			//
			// GET TARGET PATHS
			//

			if (!hasTargets)
			{
				if (!StartsWith(line, "target: "))
				{
					Log::Print(
						"Kmf file '" + kmfFile.string() + "' has a missing target key!",
						"READ_KMF",
						LogType::LOG_ERROR);

					return {};
				}

				string targetPathsString = RemoveAllFromString(line, "target: ");
				if (targetPathsString.empty())
				{
					Log::Print(
						"Kmf file '" + kmfFile.string() + "' has no assigned target path values!",
						"READ_KMF",
						LogType::LOG_ERROR);

					return {};
				}

				vector<string> targets = SplitString(targetPathsString, ", ");
				if (targets.empty())
				{
					Log::Print(
						"Kmf file '" + kmfFile.string() + "' has no assigned target path values!",
						"READ_KMF",
						LogType::LOG_ERROR);

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
					if (!exists(fullTarget))
					{
						Log::Print(
							"Kmf file '" + kmfFile.string() + "' has an invalid target '" + target + "', it was skipped.",
							"READ_KMF",
							LogType::LOG_WARNING);
					}

					kmfBlock.targets.push_back(fullTarget);
				}

				//also ensure any values actually were moved to kmf block if invalid paths were skipped
				if (kmfBlock.targets.empty())
				{
					Log::Print(
						"Kmf file '" + kmfFile.string() + "' has no assigned target path values!",
						"READ_KMF",
						LogType::LOG_ERROR);

					return {};
				}

				hasTargets = true;
			}

			//
			// GET ACTION STATE
			//

			if (!hasAction)
			{
				if (!StartsWith(line, "action: "))
				{
					Log::Print(
						"Kmf file '" + kmfFile.string() + "' has a missing action key!",
						"READ_KMF",
						LogType::LOG_ERROR);

					return {};
				}

				string actionString = RemoveAllFromString(line, "action: ");
				if (actionString.empty())
				{
					Log::Print(
						"Kmf file '" + kmfFile.string() + "' has no assigned action value!",
						"READ_KMF",
						LogType::LOG_ERROR);

					return {};
				}

				if (find(actionTypes.begin(), actionTypes.end(), actionString) == actionTypes.end())
				{
					Log::Print(
						"Kmf file '" + kmfFile.string() + "' has an invalid overwrite value!",
						"READ_KMF",
						LogType::LOG_ERROR);

					return {};
				}

				kmfBlock.action = actionString;

				hasAction = true;
			}

			//
			// FINAL CHECKUP
			//

			if (!hasOrigin
				|| !hasTargets
				|| !hasAction)
			{
				Log::Print(
					"Kmf file '" + kmfFile.string() + "' is corrupt or mistyped! One or more origins, targets or actions are missing.",
					"READ_KMF",
					LogType::LOG_ERROR);

				return {};
			}

			//
			// PUSH TO RESULT AND CLEANUP
			//

			result.push_back(kmfBlock);

			kmfBlock.origin.clear();
			kmfBlock.targets.clear();
			kmfBlock.action.clear();

			hasOrigin = false;
			hasTargets = false;
			hasAction = false;
		}
	}

	return result;
}

void HandleKMFBlock(KMF kmfBlock)
{

}