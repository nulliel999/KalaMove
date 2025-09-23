//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#include "logging/logging.hpp"
#include "core/core.hpp"

using MoveProject::Core;

using KalaHeaders::Log;

using std::cin;

int main()
{
	Core::Run();
	
	Log::Print("Press any key to exit...");
	cin.get();

	return 0;
}