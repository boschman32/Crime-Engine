#pragma once
#include <cstdio>

#if !RUN_EXAMPLES
	#ifdef PLATFORM_WINDOWS
	int main(int, char*[])
	{
		auto app = ce::CreateApplication();
		if(!app->Run())
		{
			printf("Failed to run application!\n");
		}	
		delete app;
		
		return 0;
	}
	#elif PLATFORM_SWITCH
	//Define "isatty" as switch doesn't define this but needs to be for spdlog
	//https://developer.nintendo.com/group/development/g1kr9vj6/forums/english/-/gts_message_boards/thread/290338454#934084
	extern "C" int isatty(int) { return 0; }

	extern "C" void nnMain()
	{
		auto app = ce::CreateApplication();
		if(!app->Run())
		{
			printf("Failed to run application!\n");
		}
		delete app;
	}
	#endif
#endif