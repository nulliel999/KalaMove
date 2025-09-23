//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#include "core.hpp"
#include "logging/logging.hpp"

using MoveProject::Core;

using KalaHeaders::Log;

using std::cin;

int main()
{
	Core::Run();
	
	Log::Print("\nPress 'Enter' to exit...");
	cin.get();

	return 0;
}