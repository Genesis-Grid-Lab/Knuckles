#pragma once

#include <memory>

#ifdef KE_PLATFORM_WINDOWS
#if KE_DYNAMIC_LINK
				#ifdef KE_BUILD_DLL
								#define KNUCKLES_API __declspec(dllexport)
				#else
								#define KNUCKLES_API __declspec(dllimport)
				#endif
#else
	#define KNUCKLES_API
#endif
#elif KE_PLATFORM_LINUX
#if KE_DYNAMIC_LINK
				#ifdef KE_BUILD_DLL
								#define KNUCKLES_API __attribute__((visibility("default")))
				#else 
								#define KNUCKLES_API
				#endif
#else
	#define KNUCKLES_API
#endif
#else
    #define KNUCKLES_API
    #pragma warning Unknown dynamic link import/export semantics.
#endif

#include "embed_breakpoint.h"
#ifdef KE_ENABLE_ASSERTS
	#ifdef KE_PLATFORM_WINDOWS
		#define KE_ASSERT(x, ...) { if(!(x)) {KE_ERROR("Assertion Failed: {0}", __VA_ARGS__); __debugbreak(); }}
    	#define KE_CORE_ASSERT(x, ...) { if(!(x)) { KE_CORE_ERROR("Assertion Failed: {0}", __VA_ARGS__); __debugbreak();}}
	#elif KE_PLATFORM_LINUX
		#define KE_ASSERT(x, ...) { if(!(x)) {KE_ERROR("Assertion Failed: {0}", __VA_ARGS__); EMBED_BREAKPOINT; }}
       #define KE_CORE_ASSERT(x, ...) { if(!(x)) { KE_CORE_ERROR("Assertion Failed: {0}", __VA_ARGS__); EMBED_BREAKPOINT;}}
	#endif

#endif


#define BIT(x) (1 << x)

#define KE_BIND_EVENT_FN(fn) std::bind(&fn, this, std::placeholders::_1)

namespace Knuckles {

	template<typename T>
	using Scope = std::unique_ptr<T>;

	template<typename T>
	using Ref = std::shared_ptr<T>;
}