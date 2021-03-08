#pragma once
#include <spdlog/spdlog.h>
#include <map>

namespace ce
{
	class Logger
	{
	public:

		enum class LogLevel { Debug, Info, Trace, Warn, Error, Critical, Off };

		static void SetGlobalLogLevel(LogLevel a_level);
		static std::shared_ptr<spdlog::logger> GetLogger(const std::string& a_name);
		static std::map<std::string, std::shared_ptr<spdlog::logger>> GetAllLoggers();
		static void EnableFileLogging(const std::string& a_filename);

		static void PrintMessageToConsole(const std::string& a_msg);
		static std::vector<std::string>& GetMessages();

	private:

		static Logger& GetInstance();
		static spdlog::level::level_enum ConvertLevelToSpdlogLevel(LogLevel a_level);

		LogLevel m_level = LogLevel::Debug;
		std::unique_ptr<std::string> m_fileName;
		std::map<std::string, std::shared_ptr<spdlog::logger>> m_loggers;
		std::vector<std::string> m_messages;
	};
}

#if defined(CE_DEBUG) || defined(_DEBUG)

//Engine/core logging
template<typename ... T>
constexpr void CE_CORE_INFO(T&& ... a_args) //NOLINT
{ ce::Logger::GetLogger("Core")->info(std::forward<T>(a_args)...); } 
template<typename ... T>
constexpr void CE_CORE_TRACE(T&& ... a_args) //NOLINT
{ ce::Logger::GetLogger("Core")->trace(std::forward<T>(a_args)...); }
template<typename ... T>
constexpr void CE_CORE_WARN(T&& ... a_args) //NOLINT
{ ce::Logger::GetLogger("Core")->warn(std::forward<T>(a_args)...); }
template<typename ... T>
constexpr void CE_CORE_ERROR(T&& ... a_args) //NOLINT
{ ce::Logger::GetLogger("Core")->error(std::forward<T>(a_args)...); }
template<typename ... T>
constexpr void CE_CORE_CRITICAL(T&& ... a_args) //NOLINT
{ ce::Logger::GetLogger("Core")->critical(std::forward<T>(a_args)...); }

//Application logging
template<typename ... T>
constexpr void CE_APP_INFO(T&& ... a_args) //NOLINT
{ ce::Logger::GetLogger("App")->info(std::forward<T>(a_args)...); }
template<typename ... T>
constexpr void CE_APP_TRACE(T&& ... a_args) //NOLINT
{ ce::Logger::GetLogger("App")->trace(std::forward<T>(a_args)...); }
template<typename ... T>
constexpr void CE_APP_WARN(T&& ... a_args) //NOLINT
{ ce::Logger::GetLogger("App")->warn(std::forward<T>(a_args)...); }
template<typename ... T>
constexpr void CE_APP_ERROR(T&& ... a_args) //NOLINT
{ ce::Logger::GetLogger("App")->error(std::forward<T>(a_args)...); }
template<typename ... T>
constexpr void CE_APP_CRITICAL(T&& ... a_args) //NOLINT
{ ce::Logger::GetLogger("App")->critical(std::forward<T>(a_args)...); }

//Graphics logging
template<typename ... T>
constexpr void CE_GFX_INFO(T&& ... a_args) //NOLINT
{ ce::Logger::GetLogger("GFX")->info(std::forward<T>(a_args)...); }
template<typename ... T>
constexpr void CE_GFX_TRACE(T&& ... a_args) //NOLINT
{ ce::Logger::GetLogger("GFX")->trace(std::forward<T>(a_args)...); }
template<typename ... T>
constexpr void CE_GFX_WARN(T&& ... a_args) //NOLINT
{ ce::Logger::GetLogger("GFX")->warn(std::forward<T>(a_args)...); }
template<typename ... T>
constexpr void CE_GFX_ERROR(T&& ... a_args) //NOLINT
{ ce::Logger::GetLogger("GFX")->error(std::forward<T>(a_args)...); }
template<typename ... T>
constexpr void CE_GFX_CRITICAL(T&& ... a_args) //NOLINT
{ ce::Logger::GetLogger("GFX")->critical(std::forward<T>(a_args)...); }
#else
#define CE_CORE_INFO(...)    
#define CE_CORE_TRACE(...)	 
#define CE_CORE_WARN(...)	 
#define CE_CORE_ERROR(...)	 
#define CE_CORE_CRITICAL(...)

#define CE_APP_INFO(...)     
#define CE_APP_TRACE(...)    
#define CE_APP_WARN(...)     
#define CE_APP_ERROR(...)    
#define CE_APP_CRITICAL(...) 


#define CE_GFX_INFO(...)     
#define CE_GFX_TRACE(...)	 
#define CE_GFX_WARN(...)	 
#define CE_GFX_ERROR(...)	 
#define CE_GFX_CRITICAL(...) 
#endif