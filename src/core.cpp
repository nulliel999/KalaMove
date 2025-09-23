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
#include "external/logging.hpp"
#include "external/string_helpers.hpp"

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

//Each kalamovefile block in the .kmf file
struct KMF
{
	string origin{};
	vector<string> targts{};
	bool overwrite{};
};

vector<KMF> kmfContent{};
static vector<string> allKMFFiles{};

constexpr string_view KMF_EXTENSION = ".ktf";

static vector<string> GetAllKMFFiles();
static vector<KMF> GetAllKMFContent(const string& kmfFile);
static void HandleKMFBlock(KMF kmfBlock);

namespace KalaMove
{
	void Core::Run()
	{
		vector<string> kmfFiles = GetAllKMFFiles();

		if (kmfFiles.empty())
		{
			Log::Print(
				"Did not find any .kmf files. There is nothing to copy.",
				"KMF",
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
				"KMF",
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

vector<string> GetAllKMFFiles()
{
	return{};
}

vector<KMF> GetAllKMFContent(const string& kmfFile)
{
	return{};
}

void HandleKMFBlock(KMF kmfBlock)
{

}