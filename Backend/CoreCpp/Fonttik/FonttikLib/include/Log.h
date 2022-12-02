//Copyright (C) 2022 Electronic Arts, Inc.  All rights reserved.

#pragma once
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/rotating_file_sink.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/fmt/ostr.h>

#if FMT_VERSION >= 90000
	#include <opencv2/core.hpp>
	template <> struct fmt::formatter<cv::Rect> : ostream_formatter {};
	template <> struct fmt::formatter<cv::Point> : ostream_formatter {};
#endif

namespace tik
{

class Log
{
public:

	/// <summary>
	/// Call at the end of the application to flush all pending messages
	/// </summary>
	static void ShutDown() { spdlog::shutdown(); }

	inline static std::shared_ptr<spdlog::logger>& GetCoreLogger() { return m_CoreLogger; }

	struct RotatingFileSinkParams
	{
		RotatingFileSinkParams(const char* fileName) : fileName(fileName) {}

		const char* fileName = nullptr;
		std::size_t max_size = 1024 * 1024;
		std::size_t max_files = 5;
		bool rotate_on_open = true;
	};

	/// <summary>
	/// Initializes Core Logger
	/// </summary>
	/// <param name="console">Enable/Disable logging to console</param>
	/// <param name="file">Enable/Disable logging to file</param>
	/// <param name="level">Define a level to filter logging messages by severity</param>
	/// <param name="params">If file logging enabled, provide params to configure file sink</param>
	/// <param name="pattern">Provide logging pattern, if pattern is nullptr, default spdlog logging pattern will be used</param>
	static void InitCoreLogger(bool console, bool file, int level = 0, RotatingFileSinkParams params = nullptr, const char* pattern = nullptr);

	/// <summary>
	/// Removes previous file sink in logger and adds a new one
	/// </summary>
	/// <param name="logger">logger object to modify</param>
	/// <param name="params">Params to create new file sink</param>
	/// <param name="pattern">Logging pattern, if nullptr, default spdlog logging pattern will be used</param>
	/// <param name="level">message level filter </param>
	static void SetFileLogger(std::shared_ptr<spdlog::logger> logger, RotatingFileSinkParams params, const char* pattern = nullptr, int level = -1);

	static void SetFileLogger(std::string fileName);

	/// <summary>
	/// Pops the file logger from the sinks vector (the file logger is always at the back of the vector)
	/// </summary>
	static void RemoveFileLogger();

protected:

	/// <summary>
	/// Create and return a thread safe console sink
	/// </summary>
	static std::shared_ptr<spdlog::sinks::stdout_color_sink_mt> ConsoleSink();

	/// <summary>
	/// Create and return a thread safe rotating file sink
	/// </summary>
	/// <param name="params">Params to create file sink</param>
	static std::shared_ptr<spdlog::sinks::rotating_file_sink_mt> RotatingFileSink(RotatingFileSinkParams params);

	/// <summary>
	/// Create and return a thread safe basic file sink
	/// </summary>
	/// <param name="fileName">Name of log file</param>
	static std::shared_ptr<spdlog::sinks::basic_file_sink_mt> BasicFileSink(const char* fileName);

	static std::shared_ptr<spdlog::logger> m_CoreLogger;
};

}

//Core Logger macros
#define LOG_CORE_TRACE(...)					tik::Log::GetCoreLogger()->trace(__VA_ARGS__)
#define LOG_CORE_DEBUG(...)					tik::Log::GetCoreLogger()->debug(__VA_ARGS__)
#define LOG_CORE_INFO(...)					tik::Log::GetCoreLogger()->info(__VA_ARGS__)
#define LOG_CORE_WARNING(...)				tik::Log::GetCoreLogger()->warn(__VA_ARGS__)
#define LOG_CORE_ERROR(...)					tik::Log::GetCoreLogger()->error(__VA_ARGS__)
#define LOG_CORE_CRITICAL(...)				tik::Log::GetCoreLogger()->critical(__VA_ARGS__)



