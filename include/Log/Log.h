#pragma once

#include <memory>
#include "Core/Core.h"
#define SPDLOG_FMT_EXTERNAL
#include "spdlog/spdlog.h"
#include "spdlog/fmt/ostr.h"

namespace Knuckles {

	class KNUCKLES_API Log
	{
	public:
		static void Init();

		inline static std::shared_ptr<spdlog::logger>& GetCoreLogger() { return s_CoreLogger; }
		inline static std::shared_ptr<spdlog::logger>& GetClientLogger() { return s_ClientLogger; } 
	private:
		static std::shared_ptr<spdlog::logger> s_CoreLogger;
		static std::shared_ptr<spdlog::logger> s_ClientLogger;
	};


}

// core log macro
#define KE_CORE_ERROR(...) 		::Knuckles::Log::GetCoreLogger()->error(__VA_ARGS__)
#define KE_CORE_WARN(...) 		::Knuckles::Log::GetCoreLogger()->warn(__VA_ARGS__)
#define KE_CORE_INFO(...) 		::Knuckles::Log::GetCoreLogger()->info(__VA_ARGS__)
#define KE_CORE_TRACE(...) 		::Knuckles::Log::GetCoreLogger()->trace(__VA_ARGS__)
#define KE_CORE_FATAL(...) 		::Knuckles::Log::GetCoreLogger()->fatal(__VA_ARGS__)

// Client log macro
#define KE_ERROR(...)  			::Knuckles::Log::GetClientLogger()->error(__VA_ARGS__)   
#define KE_WARN(...)   			::Knuckles::Log::GetClientLogger()->warn(__VA_ARGS__)    
#define KE_INFO(...)   			::Knuckles::Log::GetClientLogger()->info(__VA_ARGS__)    
#define KE_TRACE(...)  			::Knuckles::Log::GetClientLogger()->trace(__VA_ARGS__)   
#define KE_FATAL(...)  			::Knuckles::Log::GetClientLogger()->fatal(__VA_ARGS__)