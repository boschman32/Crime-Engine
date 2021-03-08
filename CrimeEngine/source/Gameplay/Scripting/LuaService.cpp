#include "cepch.h"
#include "Gameplay/Scripting/LuaService.h"
#include "Utils/FileSystem.h"

#include "Core/Core.h"

#ifdef PLATFORM_WINDOWS
#include <filesystem>
namespace fs = std::experimental::filesystem;
#endif

namespace ce
{
	namespace scripting
	{
		void LuaService::OnCreate()
		{
			if(!m_isInitialized)
			{
				m_luaState = sol::state();
				m_luaState.open_libraries(sol::lib::base);

				//Add an error handler to the lua state to throw any errors to lua if any happen.
				m_luaState.script(R"(
				function error_handler( error_msg )
					return "error: " .. error_msg
				end
				)");

				m_isInitialized = true;
			}
		}

		void LuaService::OnDestroy()
		{
			if(m_isInitialized)
			{
				m_luaState.collect_garbage();
				m_luaState.stack_clear();
				m_luaState = nullptr;

				m_isInitialized = false;
			}
		}

		void LuaService::FindAndRunScriptFiles()
		{
			const std::string scriptPath = "./data/scripts";

#ifdef PLATFORM_WINDOWS
			for(const auto& entry : fs::directory_iterator(scriptPath))
			{
				RunScriptFile(entry.path().string());
			}
#elif PLATFORM_SWITCH

			//TODO: Implement multi file loading for scripts in switch
#endif
		}

		void LuaService::RunScriptFile(const std::string& a_path)
		{
			std::string script;
			if(!FileSystem::ReadFileToString(a_path.c_str(), script))
			{
				CE_CORE_ASSERT_M(false, "[LuaService] Couldn't open lua script file({0})!", a_path);
			}
			const sol::protected_function_result result = m_luaState.safe_script(script);
			
			if(!result.valid() || result.status() != sol::call_status::ok)
			{
				const sol::error err = result;
				CE_CORE_ERROR("[LuaService] Lua error: {1}", a_path, err.what());
			}	
		}

		sol::load_result LuaService::LoadScriptFile(const std::string& a_path)
		{
			sol::load_result result = m_luaState.load_file(a_path);

			if(result.status() != sol::load_status::ok)
			{
				CE_CORE_ERROR("Failed to load file({0})!", a_path);

				return sol::load_result();
			}
		
			const sol::error err = result;
			CE_CORE_ASSERT_M(result.valid(), "[LuaService] Lua file({0}) contained error: {1}", a_path, err.what());
			return result;
		}

		void LuaService::UnRegisterFunction(const std::string& a_funcName)
		{
			m_luaState[a_funcName] = nullptr;
		}
	}
}