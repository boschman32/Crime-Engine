#include "cepch.h"
#include "Core/Logger.h"

#if defined(CE_DEBUG) && defined(PLATFORM_WINDOWS)
#include "Utils/imgui-sink.h"
#endif

namespace ce
{
	void Logger::SetGlobalLogLevel(LogLevel a_level)
	{
		GetInstance().m_level = a_level;

		// Set level for all active loggers
		for (auto& map : GetInstance().m_loggers)
		{
			map.second->set_level(ConvertLevelToSpdlogLevel(a_level));
		}
	}

	std::shared_ptr<spdlog::logger> Logger::GetLogger(const std::string& a_name)
	{
		const auto find = GetInstance().m_loggers.find(a_name);
		if (find == GetInstance().m_loggers.end()) 
		{
			std::vector<spdlog::sink_ptr> sinkVector;
			sinkVector.push_back(std::make_shared<spdlog::sinks::stdout_color_sink_st>());
#if defined(CE_DEBUG) && defined(PLATFORM_WINDOWS)
			sinkVector.push_back(std::make_shared<imgui_sink_mt>());
#endif

			// Check if file logging is enabled
			if (GetInstance().m_fileName)
			{
				sinkVector.push_back(std::make_shared<spdlog::sinks::basic_file_sink_st>(*GetInstance().m_fileName));
			}

			auto newLogger = std::make_shared<spdlog::logger>(a_name, begin(sinkVector), end(sinkVector));
			newLogger->set_pattern("[%T](%n) %^%v%$");
			newLogger->set_level(ConvertLevelToSpdlogLevel(GetInstance().m_level));
			newLogger->enable_backtrace(100);
			GetInstance().m_loggers[a_name] = newLogger;

			return newLogger;
		}

		return find->second;
	}

	std::map<std::string, std::shared_ptr<spdlog::logger>> Logger::GetAllLoggers()
	{
		return GetInstance().m_loggers;
	}

	void Logger::EnableFileLogging(const std::string& a_filename)
	{
		GetInstance().m_fileName = std::make_unique<std::string>(a_filename);
		for (auto& map : GetInstance().m_loggers) 
		{
			// If there is less than two sinks, add a file sink
			if (map.second->sinks().size() < 2)
			{
				map.second->sinks().push_back(std::make_shared<spdlog::sinks::basic_file_sink_st>(*GetInstance().m_fileName));
			}
		}
	}

	void Logger::PrintMessageToConsole(const std::string& a_msg)
	{
		GetInstance().m_messages.push_back(a_msg);
	}

	std::vector<std::string>& Logger::GetMessages()
	{
		return GetInstance().m_messages;
	}

	Logger& Logger::GetInstance()
	{
		static Logger s_instance;
		return s_instance;
	}

	spdlog::level::level_enum Logger::ConvertLevelToSpdlogLevel(LogLevel a_level)
	{
		switch (a_level)
		{
		case LogLevel::Debug:
		{
			return spdlog::level::debug;
		}
		case LogLevel::Trace:
		{
			return spdlog::level::trace;
		}
		case LogLevel::Info:
		{
			return spdlog::level::info;
		}
		case LogLevel::Warn:
		{
			return spdlog::level::warn;
		}
		case LogLevel::Error:
		{
			return spdlog::level::err;
		}
		case LogLevel::Critical:
		{
			return spdlog::level::critical;
		}
		case LogLevel::Off:
		{
			return spdlog::level::off;
		}
		default:
		{
			return spdlog::level::debug;
		}
		}
	}
}
