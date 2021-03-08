#pragma once

#include "Utils/Editor/EditorConsole.h"
#include "spdlog/sinks/base_sink.h"

template<typename Mutex>
class imgui_sink : public spdlog::sinks::base_sink <Mutex>
{
protected:

	void sink_it_(const spdlog::details::log_msg& a_msg) override // NOLINT: Overrides library class, therefore cannot change function name
	{
		spdlog::memory_buf_t formatted;
		spdlog::sinks::base_sink<Mutex>::formatter_->format(a_msg, formatted);
		ce::Logger::PrintMessageToConsole(fmt::to_string(formatted));
	}

	void flush_() override {} // NOLINT: Overrides library class, therefore cannot change function name

private:

	ce::editor::IEditorWindow* m_editorConsole = nullptr;
};

#include "spdlog/details/null_mutex.h"
#include <mutex>
using imgui_sink_mt = imgui_sink<std::mutex>;
using imgui_sink_st = imgui_sink<spdlog::details::null_mutex>;