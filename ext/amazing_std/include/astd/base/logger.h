#pragma once

#include "macro.h"
#include <format>
#include <string>
#include <iostream>

AMAZING_NAMESPACE_BEGIN

class Logger
{
public:
	enum class Level
	{
		e_info,
		e_warning,
		e_error
	};

	template<typename... Args>
	static void log(const Level& level, const char* loc, const std::string& message, Args&&... args);
};

template<typename... Args>
void Logger::log(const Level& level, const char* loc, const std::string& message, Args&&... args)
{
	std::string msg;
	if constexpr (sizeof...(Args) > 0)
		msg = std::vformat(message, std::make_format_args(args...));
	else
		msg = message;
	switch (level)
	{
	case Level::e_info:
		std::cout << "[" << loc << ", Info]: " << msg << std::endl;
		break;
	case Level::e_warning:
		std::cerr << "[" << loc << ", Warning]: " << msg << std::endl;
		break;
	case Level::e_error:
		const std::string format = std::format("[{}, Error]: {}", loc, msg);
		std::cerr << format << std::endl;
		throw std::runtime_error(format);
	}
}

#if defined(_DEBUG) || defined(DEBUG)
#define LOG_INFO(loc, ...)		Logger::log(Logger::Level::e_info, loc __VA_OPT__(,) __VA_ARGS__)
#define LOG_WARNING(loc, ...)	Logger::log(Logger::Level::e_warning, loc __VA_OPT__(,) __VA_ARGS__)
#define LOG_ERROR(loc, ...)		Logger::log(Logger::Level::e_error, loc __VA_OPT__(,) __VA_ARGS__)

#define ASSERT(expr, loc, ...)  if (!(expr)) LOG_ERROR(loc, __VA_ARGS__);

#define CONTAINER_LOG_INFO(...)		LOG_INFO("Container", __VA_ARGS__)
#define CONTAINER_LOG_WARNING(...)	LOG_WARNING("Container", __VA_ARGS__)
#define CONTAINER_LOG_ERROR(...)	LOG_ERROR("Container", __VA_ARGS__)
#define CONTAINER_ASSERT(expr, ...)	ASSERT(expr, "Container", __VA_ARGS__)


#define RENDERING_LOG_INFO(...)		LOG_INFO("Rendering", __VA_ARGS__)
#define RENDERING_LOG_WARNING(...)	LOG_WARNING("Rendering", __VA_ARGS__)
#define RENDERING_LOG_ERROR(...)	LOG_ERROR("Rendering", __VA_ARGS__)
#define RENDERING_ASSERT(expr, ...)	ASSERT(expr, "Rendering", __VA_ARGS__)
#else
#define LOG_INFO(loc, ...)	reinterpret_cast<void*>(0)
#define LOG_WARNING(loc, ...) reinterpret_cast<void*>(0)
#define LOG_ERROR(loc, ...)	reinterpret_cast<void*>(0)

#define ASSERT(expr, loc, ...) (expr)
#endif

#define CONTAINER_LOG_INFO(...)		LOG_INFO("Container", __VA_ARGS__)
#define CONTAINER_LOG_WARNING(...)	LOG_WARNING("Container", __VA_ARGS__)
#define CONTAINER_LOG_ERROR(...)	LOG_ERROR("Container", __VA_ARGS__)
#define CONTAINER_ASSERT(expr, ...)	ASSERT(expr, "Container", __VA_ARGS__)


#define RENDERING_LOG_INFO(...)		LOG_INFO("Rendering", __VA_ARGS__)
#define RENDERING_LOG_WARNING(...)	LOG_WARNING("Rendering", __VA_ARGS__)
#define RENDERING_LOG_ERROR(...)	LOG_ERROR("Rendering", __VA_ARGS__)
#define RENDERING_ASSERT(expr, ...)	ASSERT(expr, "Rendering", __VA_ARGS__)



AMAZING_NAMESPACE_END