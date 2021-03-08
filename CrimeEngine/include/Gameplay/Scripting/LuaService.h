#pragma once
#include "Utils/IService.h"
#include "Core/Logger.h"

#include <sol/sol.hpp>

namespace ce
{
	namespace scripting
	{
		class LuaService : public IService
		{
		public:
			void OnCreate() override;
			void OnDestroy() override;

			 /**
	          * \brief Run and load a lua script file.
	          * 
	          * \param a_path The path to the lua script.
	          *
	         **/
			void RunScriptFile(const std::string& a_path);
			
			 /**
	          * \brief Only load a lua script file, does not execute it.
	          * 
	          * \param a_path The path to the lua script.
	          *
	         **/
			sol::load_result LoadScriptFile(const std::string& a_path);

			 /**
	          * \brief Get the lua state.
	          * 
	          * \return The lua state.
	          *
	         **/
			sol::state& GetState() { return m_luaState; }

			 /**
	          * \brief Invoke a lua function,
	          *			will check if the function exists and also check if the result given back by the function is valid.
	          * 
	          * \param a_funcName The name of the function inside lua.
	          * \param a_args arguments passed to the lua function.
	          *
	          * \return The value that the lua function returns (is void by default).
	          * 
	         **/
			template<typename RetType = sol::object, typename ... Args>
			RetType InvokeFunction(const std::string& a_funcName, Args ... a_args);

			/**
	          * \brief Invoke an "unsafe" lua function,
	          *			this does not check for any errors and will crash if they happen,
	          *			but is faster in terms of performance then the normal invoke function.
	          * 
	          * \param a_funcName The name of the function inside lua.
	          * \param a_args arguments passed to the lua function.
	          *
	          * \return The value that the lua function returns (is void by default).
	          * 
	         **/
			template<typename RetType = sol::object, typename ... Args>
			RetType InvokeUnsafeFunction(const std::string& a_funcName, Args ... a_args);

			/**
	          * \brief Register a callback for a lua script to call.
	          * 
	          * \param a_funcName The name of the callback will appear inside lua.
	          * \param a_callback The "functor" to be called from lua.
	          *		(can be a lambda, std::function, global function pointer and member function pointer).
	          *
	         **/
			template<typename Functor>
			void RegisterFunction(const std::string& a_funcName, Functor&& a_callback);

			/**
	          * \brief Un-register a function callback from the lua state.
	          * 
	          * \param a_funcName The name that can no longer be called from lua.
	          *
	         **/
			void UnRegisterFunction(const std::string& a_funcName);

			/**
	          * \brief Find and run all script files inside the root script directory.
	          * 
	         **/
			void FindAndRunScriptFiles();
		private:
			sol::state m_luaState { nullptr };
			bool m_isInitialized { false };

			template<typename T>
			sol::type GetLuaType();
		};

		template<typename RetType, typename ... Args>
		RetType LuaService::InvokeFunction(const std::string& a_funcName, Args ... a_args)
		{
			if(!m_isInitialized)
			{
				return RetType();
			}
			
			const sol::object funcObj = m_luaState[a_funcName];

			//Are we calling a valid function.
			if(funcObj.valid() && funcObj.get_type() == sol::type::function &&
				funcObj.is<std::function<RetType(Args...)>>())
			{
				sol::protected_function func = funcObj;
				func.error_handler = m_luaState["error_handler"];
			
				sol::protected_function_result result = func(std::forward<Args>(a_args)...);

				//Call when correct
				if(result.valid())
				{
					if(result.get_type() != GetLuaType<RetType>())
					{
						CE_CORE_ERROR("[LuaService] Function({0}) returned string instead of {1}", a_funcName, typeid(RetType).name());
						return RetType();
					}
					return result.get<RetType>();
				}
				const sol::error err = result;
				CE_CORE_ERROR("[LuaService] Error in lua function({0}): \n{1}", a_funcName, err.what());
				return RetType();
			}
			
			CE_CORE_CRITICAL("[LuaService] Could't run lua function({0})!", a_funcName);
			return RetType();
		}

		template <typename RetType, typename ... Args>
		RetType LuaService::InvokeUnsafeFunction(const std::string& a_funcName, Args ... a_args)
		{
			if(!m_isInitialized)
			{
				return RetType();
			}

			const sol::object funcObj = m_luaState[a_funcName];
			sol::unsafe_function func = funcObj;
			sol::unsafe_function_result result = func(std::forward<Args>(a_args)...);
			return result.get<RetType>();
		}
		
		template<typename Functor>
		void LuaService::RegisterFunction(const std::string& a_funcName, Functor&& a_callback)
		{
			if(!m_isInitialized)
			{
				return;
			}

			m_luaState.set_function(a_funcName, std::forward<Functor>(a_callback));
		}

		template <typename T>
		sol::type LuaService::GetLuaType()
		{
			if(std::is_same<T, std::string>::value)
			{
				return sol::type::string;
			}
			if(std::is_same<T, bool>::value)
			{
				return sol::type::boolean;
			}
			if(std::is_same<T, int>::value)
			{
				return sol::type::number;
			}
			if(std::is_same<T, float>::value)
			{
				return sol::type::number;
			}
			return sol::type::none;
		}

	}
}
