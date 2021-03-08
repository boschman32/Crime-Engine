#include "CppUnitTest.h"

#include <map>

#include "Core/Logger.h"
#include "Gameplay/Scripting/LuaService.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace EngineTests
{
	TEST_CLASS(Resources)
	{
	public:

		TEST_METHOD(Map_Resource)
		{
			//Map* map = new Map("StreamlinedMastermind/data/map/testMap.tmx");
			//Assert::IsTrue(map->m_mapResource->IsLoaded());
		}
	};


	TEST_CLASS(LuaTesting)
	{
	public:
		TEST_METHOD_INITIALIZE(Lua_Init)
		{
			m_luaService = ce::ServiceManager::MakeService<ce::scripting::LuaService>();

			//Create test functions.
			m_luaService->GetState().script(R"(
			function to_test_return()
				return "Hello World!"
			end

			function func_with_args(value)
				return value + 5
			end

			function call_back_test_no_args()
				call_the_callback()
			end

			function call_back_test_with_args(value)
				call_the_callback(value + 17)
			end
			)");
		}

		TEST_METHOD(Lua_Get_State)
		{
			sol::state& state = m_luaService->GetState();

			Assert::IsNotNull(&state, L"Get Lua state returned null!");
		}

		TEST_METHOD(Lua_Invoke_Function_Return)
		{
			const std::string functionName = "to_test_return";
			const std::string expectedResult = "Hello World!";
			
			const std::string result = m_luaService->InvokeFunction<std::string>(functionName);

			Assert::AreEqual(result, expectedResult, L"Invoking lua function doesn't return same result!");
		}

		TEST_METHOD(Lua_Invoke_Function_Return_Wrong_Type)
		{
			const std::string functionName = "to_test_return";
			const std::string expectedResult = "Hello World!";
			
			const int result = m_luaService->InvokeFunction<int>(functionName);

			Assert::IsTrue((result == 0), L"Called invoke function with wrong type should return 0 but returned something else!");
		}

		TEST_METHOD(Lua_Invoke_Function_With_Args)
		{
			const std::string functionName = "func_with_args";
			const int expectedResult = 5;
			const int argument = 0;
			
			const int result = m_luaService->InvokeFunction<int>(functionName, argument);

			Assert::AreNotSame(result, expectedResult, L"Called invoke function with wrong type should return 0 but returned something else!");
		}

		TEST_METHOD(Lua_Register_Callback_No_Args)
		{
			const std::string functionName = "call_the_callback";
			const std::string functionToCall = "call_back_test_no_args";
			bool result = false;
			std::function<void()> callback = [&result]()
			{
				result = true;
			};

			m_luaService->RegisterFunction(functionName, callback);
			m_luaService->InvokeFunction(functionToCall);

			Assert::IsTrue(result, L"Expected result from callback to return true");
		}

		TEST_METHOD(Lua_Register_Callback_With_Args)
		{
			const std::string functionName = "call_the_callback";
			const std::string functionToCall = "call_back_test_with_args";
			const int expectedResult = 17;
			int result = 0;
			std::function<void(int)> callback = [&result](int a_par)
			{
				result = a_par;
			};
			const int argument = 0;

			m_luaService->RegisterFunction(functionName, callback);
			m_luaService->InvokeFunction(functionToCall, argument);

			Assert::AreNotSame(result, expectedResult, L"Returned argument didn't return correct result!");
		}
	private:
		ce::scripting::LuaService* m_luaService { nullptr };

	};
}