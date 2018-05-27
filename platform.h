#pragma once
#include <stdint.h>
#include <stddef.h>
#include <stdarg.h>

class Platform {
public:
	static uint64_t GetCurrentThreadId(void);
	static void* LoadModule(const char* path);
	static void* LoadFunction(void* module, const char* signature);
	static bool GetCurrentModulePath(char* buffer, size_t size);
	static bool GetWorkingDirectory(char* buffer, size_t size);
	static bool MakeDirectory(const char* path);
	static int64_t GetMillisTicks(void);
	static int64_t GetMillisAbsolute(void);
	static int vsnprintf(char* buffer, size_t size, const char* format, va_list args);
	static bool snprintf(char* buffer, size_t size, const char* format, ...);
	static bool strncpy(char* destination, const char* source, size_t size);
	static void* MutexCreate(void);
	static void MutexLock(void* handle);
	static void MutexUnlock(void* handle);
	static void MutexDelete(void* handle);
};
