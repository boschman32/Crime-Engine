#include "expch.h"

#include <iostream>

#include "ExampleLuaScripting.h"
#include "ExampleMapScene.h"
#include "ExampleResources.h"
#include "ExampleRender.h"
#include "ExampleAnimation.h"
#include "ExampleEvents.h"

#define RUN_LUA_EXAMPLE 0
#define RUN_RESOURCE_EXAMPLE 0
#define RUN_MAP_SCENE_EXAMPLE 0
#define RUN_RENDER_EXAMPLE 0
#define RUN_ANIMATION_EXAMPLE 0
#define RUN_EVENTS_EXAMPLE 1

#ifdef PLATFORM_WINDOWS
int main(int, char*[])
{
#if RUN_LUA_EXAMPLE
	ce::examples::ExampleLuaScripting luaScripting;
	luaScripting.RunExample();
#endif
#if RUN_RESOURCE_EXAMPLE
	ce::examples::ExampleResources resources;
	resources.RunExample();
#endif
#if RUN_MAP_SCENE_EXAMPLE
	ce::examples::ExampleMapScene scene;
	scene.RunExample();
#endif
#if RUN_RENDER_EXAMPLE
	ce::examples::ExampleRender scene;
	scene.RunExample();
#endif
#if RUN_ANIMATION_EXAMPLE
	ce::examples::ExampleAnimation scene;
	scene.RunExample();
#endif
#if RUN_EVENTS_EXAMPLE
	ce::examples::ExampleEvents events;
	events.RunExample();
#endif

	std::cin.get();
}
#elif PLATFORM_SWITCH
//Define "isatty" as switch doesn't define this but needs to be for spdlog
//https://developer.nintendo.com/group/development/g1kr9vj6/forums/english/-/gts_message_boards/thread/290338454#934084
extern "C" int isatty(int) { return 0; }

extern "C" void nnMain()
{
	ce::examples::ExampleResources resources;
	resources.RunExample();

	ce::examples::ExampleLuaScripting luaScripting;
	luaScripting.RunExample();
}
#endif