#include "../platform.h"
#include "../logging.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif
#include <dlfcn.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <string.h>

uint64_t Platform::GetCurrentThreadId(void) {
	return (uint64_t)syscall(SYS_gettid);
}

void* Platform::LoadModule(const char* path) {
	void* module = dlopen(path, RTLD_NOW | RTLD_LOCAL | RTLD_DEEPBIND);

	if (module == nullptr) {
		Logging::Log(LogError, __FUNCTION__, "Loading module %s failed with error: %s", path, dlerror());
	}

	return module;
}

void* Platform::LoadFunction(void* module, const char* signature) {
	void* result = dlsym(module, signature);

	if (result == nullptr) {
		Logging::Log(LogError, __FUNCTION__, "Getting symbol %s failed with error: %s", signature, dlerror());
	}

	return result;
}

bool Platform::GetCurrentModulePath(char* buffer, size_t size) {
	Dl_info addr_info;
	
	if (!dladdr((void*)Platform::GetCurrentModulePath, &addr_info)) {
		return false;
	}
	
	return Platform::strncpy(buffer, addr_info.dli_fname, size);
}

bool Platform::GetWorkingDirectory(char* buffer, size_t size) {
	return getcwd(buffer, size) != nullptr;
}

bool Platform::MakeDirectory(const char* path) {
	return mkdir(path, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) != -1;
}

int64_t Platform::GetMillisTicks(void) {
	struct timespec current_time;
	clock_gettime(CLOCK_MONOTONIC, &current_time);
	
	return ((uint64_t)current_time.tv_sec) * 1000ULL + ((uint64_t)current_time.tv_nsec) / 1000000ULL;
}

int64_t Platform::GetMillisAbsolute(void) {
	struct timespec current_time;
	clock_gettime(CLOCK_REALTIME, &current_time);
	
	return ((uint64_t)current_time.tv_sec) * 1000ULL + ((uint64_t)current_time.tv_nsec) / 1000000ULL;
}

int Platform::vsnprintf(char* buffer, size_t size, const char* format, va_list args) {
	if (size > 0) {
		buffer[0] = '\0';
	}

	return ::vsnprintf(buffer, size, format, args);
}

bool Platform::snprintf(char* buffer, size_t size, const char* format, ...) {
	va_list args;
	va_start(args, format);
	int result = vsnprintf(buffer, size, format, args);
	va_end(args);

	return result >= 0 && (size_t)result < size;
}

bool Platform::strncpy(char* destination, const char* source, size_t size) {
	if (size == 0) {
		return false;
	}
	
	::strncpy(destination, source, size);
	char last = destination[size - 1];
	destination[size - 1] = '\0';
	
	if (strlen(destination) == size - 1 && last != '\0') {
		return false;
	}
	
	return true;
}

struct MutexLinux {
	pthread_mutex_t mutex;
};

void* Platform::MutexCreate(void) {
	MutexLinux* mutex = new MutexLinux();
	pthread_mutex_init(&mutex->mutex, NULL);
	return mutex;
}

void Platform::MutexLock(void* handle) {
	MutexLinux* mutex = (MutexLinux*)handle;
	pthread_mutex_lock(&mutex->mutex);
}

void Platform::MutexUnlock(void* handle) {
	MutexLinux* mutex = (MutexLinux*)handle;
	pthread_mutex_unlock(&mutex->mutex);
}

void Platform::MutexDelete(void* handle) {
	MutexLinux* mutex = (MutexLinux*)handle;
	pthread_mutex_destroy(&mutex->mutex);
	delete mutex;
}
