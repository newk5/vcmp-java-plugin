#include "../platform.h"
#include "../logging.h"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <crtdbg.h>
#include <stdio.h>
#include <errno.h>
#include <stdarg.h>

static HMODULE currentModule = nullptr;

BOOL WINAPI DllMain(HINSTANCE module, DWORD reason, LPVOID reserved) {
	currentModule = module;
	return TRUE;
}

uint64_t Platform::GetCurrentThreadId(void) {
	return ::GetCurrentThreadId();
}

void* Platform::LoadModule(const char* path) {
	HMODULE module = LoadLibraryA(path);

	if (module == nullptr) {
		Logging::Log(LogError, __FUNCTION__, "Loading module %s failed with error: %d", path, GetLastError());
	}

	return module;
}

void* Platform::LoadFunction(void* module, const char* signature) {
	void* result = GetProcAddress((HMODULE)module, signature);

	if (result == nullptr) {
		Logging::Log(LogError, __FUNCTION__, "Getting symbol %s failed with error: %d", signature, GetLastError());
	}

	return result;
}

bool Platform::GetCurrentModulePath(char* buffer, size_t size) {
	return GetModuleFileNameA(currentModule, buffer, (DWORD)size) != 0;
}

bool Platform::GetWorkingDirectory(char* buffer, size_t size) {
	return GetCurrentDirectoryA((DWORD)size, buffer) != 0;
}

bool Platform::MakeDirectory(const char* path) {
	return CreateDirectoryA(path, NULL) != 0;
}

int64_t Platform::GetMillisTicks(void) {
	return GetTickCount();
}

union FileTimeInt64 {
	FILETIME fileTime;
	int64_t value;
};

int64_t Platform::GetMillisAbsolute(void) {
	static const int64_t startTime = 116444736000000000LL;

	FileTimeInt64 fileTime;
	GetSystemTimeAsFileTime(&fileTime.fileTime);

	return (fileTime.value - startTime) / 10000LL;
}

int Platform::vsnprintf(char* buffer, size_t size, const char* format, va_list args) {
	static int initialised = 0;

	if (!initialised) {
		initialised = 1;
		_CrtSetDebugFillThreshold(0);
	}

	if (size > 0) {
		buffer[0] = '\0';
	}

	int result = vsnprintf_s(buffer, size, _TRUNCATE, format, args);

	if (result == -1 && errno == 0) {
		return (int)size + 1;
	}

	return result;
}

bool Platform::snprintf(char* buffer, size_t size, const char* format, ...) {

	va_list args;
	va_start(args, format);
	int result = vsnprintf(buffer, size, format, args);
	va_end(args);

	return result >= 0 && (size_t)result < size;
}

bool Platform::strncpy(char* destination, const char* source, size_t size) {
	return strncpy_s(destination, size, source, _TRUNCATE) == 0;
}

struct MutexWindows {
	CRITICAL_SECTION criticalSection;
};

void* Platform::MutexCreate(void) {
	MutexWindows* mutex = new MutexWindows();
	InitializeCriticalSection(&mutex->criticalSection);
	return mutex;
}

void Platform::MutexLock(void* handle) {
	MutexWindows* mutex = (MutexWindows*)handle;
	EnterCriticalSection(&mutex->criticalSection);
}

void Platform::MutexUnlock(void* handle) {
	MutexWindows* mutex = (MutexWindows*)handle;
	LeaveCriticalSection(&mutex->criticalSection);
}

void Platform::MutexDelete(void* handle) {
	MutexWindows* mutex = (MutexWindows*)handle;
	DeleteCriticalSection(&mutex->criticalSection);
	delete mutex;
}
