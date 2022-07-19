//Copyright (C) 2022 Electronic Arts, Inc.  All rights reserved.

#include "Log.h"

namespace tin
{
	std::shared_ptr<spdlog::logger> Log::m_CoreLogger;

	void Log::InitCoreLogger(bool console, bool file, int level, RotatingFileSinkParams params, const char* pattern)
	{
		//Init Core Logger
		m_CoreLogger = std::make_shared<spdlog::logger>("CoreLogger");

		//add sinks to logger
		if (console)
		{
			m_CoreLogger->sinks().push_back(ConsoleSink());
		}
		if (file)
		{
			m_CoreLogger->sinks().push_back(RotatingFileSink(params));
		}
		if (pattern != nullptr) //otherwise spdlog default log messages
		{
			m_CoreLogger->set_pattern(pattern);
		}
		m_CoreLogger->set_level((spdlog::level::level_enum)level);
	}

	void Log::SetFileLogger(std::shared_ptr<spdlog::logger> logger, RotatingFileSinkParams params, const char* pattern, int level)
	{
		auto sink = RotatingFileSink(params);

		if (pattern != nullptr)
		{
			sink->set_pattern(pattern);
		}
		if (level != -1)
		{
			sink->set_level((spdlog::level::level_enum)level);
		}
		logger->sinks().push_back(sink);
	}

	void Log::SetFileLogger(std::string fileName)
	{
		SetFileLogger(m_CoreLogger, RotatingFileSinkParams(fileName.c_str()), "%v", 2);
	}

	void Log::RemoveFileLogger()
	{
		if (m_CoreLogger != nullptr && !m_CoreLogger->sinks().size() > 1) //ensures not removing the console log
		{
			m_CoreLogger->sinks().pop_back();
		}
	}

	std::shared_ptr<spdlog::sinks::stdout_color_sink_mt> Log::ConsoleSink()
	{
		return std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
	}

	std::shared_ptr<spdlog::sinks::rotating_file_sink_mt> Log::RotatingFileSink(RotatingFileSinkParams params)
	{
		return std::make_shared<spdlog::sinks::rotating_file_sink_mt>(params.fileName, params.max_size, params.max_files, params.rotate_on_open);
	}

	std::shared_ptr<spdlog::sinks::basic_file_sink_mt> Log::BasicFileSink(const char* fileName)
	{
		return std::make_shared<spdlog::sinks::basic_file_sink_mt>(fileName);
	}
}
