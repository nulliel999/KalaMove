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
#include "external/log_utils.hpp"
#include "external/string_utils.hpp"
#include "external/file_utils.hpp"

using KalaHeaders::Log;
using KalaHeaders::LogType;
using namespace KalaHeaders;

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

static path thisPath{};

constexpr string_view KMF_VERSION_NUMBER = "1.0";
constexpr string_view KMF_VERSION_NAME = "#KMF VERSION 1.0";
constexpr string_view KMF_EXTENSION = ".ktf";

static const array<string, 6> actionTypes =
{
	"move",      //force copy file or folder to all paths except last, move to last
	"copy",      //copy file or folder to target only if target does not already have this file
	"forcecopy", //copy file or folder to target and overwrite
	"rename",    //rename file or folder in local dir
	"delete",    //delete file or folder
	"create"     //create new directory
};

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
	void Core::Run(int argc, char* argv[])
	{
		ostringstream oss{};

		oss << "==============================\n"
			<< "== KALAMOVE 1.0\n"
			<< "==============================\n";

		Log::Print(oss.str());

		vector<path> kmfFiles{};

		auto IsValidKMFPath = [](path kmfPath)
			{
				return
					is_regular_file(kmfPath)
					&& kmfPath.extension() == ".kmf";
			};

		if (argc == 1)
		{
			Log::Print("--- Running in MANUAL mode ---\n");

			for (const auto& file : directory_iterator(current_path()))
			{
				path kmfPath = file;

				if (IsValidKMFPath(kmfPath)) kmfFiles.push_back(kmfPath);
			}
		}
		else
		{
			Log::Print("--- Running in PARAM mode ---\n");

			for (int i = 1; i < argc; ++i)
			{
				path kmfPath = argv[i];

				if (IsValidKMFPath(kmfPath)) kmfFiles.push_back(kmfPath);
			}
		}

		if (kmfFiles.empty())
		{
			Log::Print(
				"Did not find any .kmf files. There is nothing to copy.",
				"GET_KMF",
				LogType::LOG_ERROR);

			Exit();
		}
		else
		{
			Log::Print(
				"\nFound '" + to_string(kmfFiles.size()) + "' .kmf files.\n",
				"GET_KMF",
				LogType::LOG_INFO);
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

vector<KMF> GetAllKMFContent(path kmfFile)
{
	vector<KMF> result{};

	thisPath = kmfFile.parent_path();

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

	auto ClearContent = [&]()
		{
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

			if (!foundVersion)
			{
				return{};
			}

			if (isLastLine)
			{
				Log::Print(
					"Kmf file '" + kmfFile.string() + "' has no content after line '" + to_string(lineNumber) + "'!",
					"READ_KMF",
					LogType::LOG_ERROR);

				return{};
			}

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

				return {};
			}

			string originPathString = RemoveAllFromString(line, "origin: ");
			if (originPathString.empty())
			{
				Log::Print(
					"Kmf file '" + kmfFile.string() + "' has an origin path with no assigned value at line '" + to_string(lineNumber) + "'!",
					"READ_KMF",
					LogType::LOG_ERROR);

				return {};
			}

			bool isAbsolute = StartsWith(originPathString, "@@");

			path originPath{};

			//add path relative to exe as prefix if not absolute
			if (!isAbsolute) originPath = thisPath / originPathString;
			else
			{
				originPathString = ReplaceAllFromString(originPathString, "@@", "");
				originPath = originPathString;
			}

#ifdef _WIN32
			originPath = ReplaceAllFromString(originPath.string(), "@", "\\");
#else
			originPath = ReplaceAllFromString(originPath.string(), "@", "/");
#endif

			if (!exists(originPath))
			{
				Log::Print(
					"Kmf file '" + kmfFile.string() + "' has an origin path '" + originPathString + "' at line '" + to_string(lineNumber) + "' that does not exist!",
					"READ_KMF",
					LogType::LOG_ERROR);

				return {};
			}

			Log::Print(
				"Kmf file '" + kmfFile.stem().string() + "' has a correct origin '" + originPathString + "' at line '" + to_string(lineNumber) + "'.",
				"READ_KMF",
				LogType::LOG_DEBUG);

			kmfBlock.origin = originPath;
			hasOrigin = true;

			if (isLastLine)
			{
				result.push_back(kmfBlock);
				ClearContent();
			}

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

				return {};
			}

			string targetPathsString = RemoveAllFromString(line, "target: ");
			if (targetPathsString.empty())
			{
				Log::Print(
					"Kmf file '" + kmfFile.string() + "' has no assigned target path values at line '" + to_string(lineNumber) + "'!",
					"READ_KMF",
					LogType::LOG_ERROR);

				return {};
			}

			vector<string> targets = SplitString(targetPathsString, ", ");
			if (targets.empty())
			{
				Log::Print(
					"Kmf file '" + kmfFile.string() + "' has no assigned target path values at line '" + to_string(lineNumber) + "'!",
					"READ_KMF",
					LogType::LOG_ERROR);

				return {};
			}

			for (const auto& target : targets)
			{
				string correctTarget = target;

				bool isAbsolute = StartsWith(correctTarget, "@@");

				path fullTarget{};

				//add path relative to exe as prefix if not absolute
				if (!isAbsolute) fullTarget = thisPath / correctTarget;
				else
				{
					correctTarget = ReplaceAllFromString(correctTarget, "@@", "");
					fullTarget = correctTarget;
				}

#ifdef _WIN32
				fullTarget = ReplaceAllFromString(fullTarget.string(), "@", "\\");
#else
				fullTarget = ReplaceAllFromString(fullTarget.string(), "@", "/");
#endif

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

				return {};
			}

			hasTargets = true;

			if (isLastLine)
			{
				result.push_back(kmfBlock);
				ClearContent();
			}
			
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

				return {};
			}

			string actionString = RemoveAllFromString(line, "action: ");
			if (actionString.empty())
			{
				Log::Print(
					"Kmf file '" + kmfFile.string() + "' has no assigned action value at line '" + to_string(lineNumber) + "'!",
					"READ_KMF",
					LogType::LOG_ERROR);

				return {};
			}

			if (find(actionTypes.begin(), actionTypes.end(), actionString) == actionTypes.end())
			{
				Log::Print(
					"Kmf file '" + kmfFile.string() + "' has an invalid action value '" + actionString + "' at line '" + to_string(lineNumber) + "'!",
					"READ_KMF",
					LogType::LOG_ERROR);

				return {};
			}

			kmfBlock.action = actionString;
			hasAction = true;

			result.push_back(kmfBlock);
			ClearContent();

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
				"Kmf file '" + kmfFile.string() + "' has no assigned valid targets at line '" + targetLine + "'!",
				"READ_KMF",
				LogType::LOG_ERROR);

			return{};
		}

		if (kmf.action.empty())
		{
			string targetLine = to_string(kmf.line + 2);

			Log::Print(
				"Kmf file '" + kmfFile.string() + "' has no assigned action at line '" + targetLine + "'!",
				"READ_KMF",
				LogType::LOG_ERROR);

			return{};
		}
	}

	return result;
}

void HandleKMFBlock(KMF kmfBlock)
{
	Log::Print(
		"Started with action '" + kmfBlock.action + "' for origin '" + kmfBlock.origin.string() + "'.",
		"HANDLE_KMF",
		LogType::LOG_DEBUG);

	for (const auto& target : kmfBlock.targets)
	{
		if (kmfBlock.action == "copy")
		{
			ostringstream success{};

			if (exists(target))
			{
				success << "Skipped copying to target '" << target << "' because it already exists.";

				Log::Print(
					success.str(),
					"HANDLE_KMF",
					LogType::LOG_SUCCESS);

				continue;
			}

			string result = CopyPath(
				kmfBlock.origin,
				target);

			if (!result.empty())
			{
				Log::Print(
					result,
					"HANDLE_KMF",
					LogType::LOG_ERROR);

				return;
			}

			success << "Copied origin '" << kmfBlock.origin << "' to target '" << target << ".";

			Log::Print(
				success.str(),
				"HANDLE_KMF",
				LogType::LOG_SUCCESS);
		}
		else if (kmfBlock.action == "forcecopy")
		{
			string result = CopyPath(
				kmfBlock.origin,
				target,
				true);

			if (!result.empty())
			{
				Log::Print(
					result,
					"HANDLE_KMF",
					LogType::LOG_ERROR);

				return;
			}

			ostringstream success{};
			success << "Force copied origin '" << kmfBlock.origin << "' to target '" << target << ".";

			Log::Print(
				success.str(),
				"HANDLE_KMF",
				LogType::LOG_SUCCESS);
		}
		else if (kmfBlock.action == "move")
		{
			if (target != kmfBlock.targets.back())
			{
				string result = CopyPath(
					kmfBlock.origin,
					target,
					true);

				if (!result.empty())
				{
					Log::Print(
						result,
						"HANDLE_KMF",
						LogType::LOG_ERROR);

					return;
				}
			}
			else
			{
				string result = MovePath(
					kmfBlock.origin,
					target);

				if (!result.empty())
				{
					Log::Print(
						result,
						"HANDLE_KMF",
						LogType::LOG_ERROR);

					return;
				}
			}

			ostringstream success{};
			success << "Moved origin '" << kmfBlock.origin << "' to target '" << target << ".";

			Log::Print(
				success.str(),
				"HANDLE_KMF",
				LogType::LOG_SUCCESS);
		}
		else if (kmfBlock.action == "rename")
		{
			string result = RenamePath(
				target,
				kmfBlock.origin.stem().string());

			if (!result.empty())
			{
				Log::Print(
					result,
					"HANDLE_KMF",
					LogType::LOG_ERROR);

				return;
			}

			ostringstream success{};
			success << "Renamed origin '" << target << "' to '" << kmfBlock.origin.stem() << ".";

			Log::Print(
				success.str(),
				"HANDLE_KMF",
				LogType::LOG_SUCCESS);
		}
		else if (kmfBlock.action == "delete")
		{
			string result = DeletePath(
				target);

			if (!result.empty())
			{
				Log::Print(
					result,
					"HANDLE_KMF",
					LogType::LOG_ERROR);

				return;
			}

			ostringstream success{};
			success << "Deleted target '" << target << "'.";

			Log::Print(
				success.str(),
				"HANDLE_KMF",
				LogType::LOG_SUCCESS);
		}
		else if (kmfBlock.action == "create")
		{
			string result = CreateDirectory(
				target);

			if (!result.empty())
			{
				Log::Print(
					result,
					"HANDLE_KMF",
					LogType::LOG_ERROR);

				return;
			}

			ostringstream success{};
			success << "Created new target '" << target << "'.";

			Log::Print(
				success.str(),
				"HANDLE_KMF",
				LogType::LOG_SUCCESS);
		}
	}
}