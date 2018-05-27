#include "logging.h"
#include <string.h>
#include <stdarg.h>
#include <stdio.h>

#include "platform.h"

void* Logging::loggingLock = nullptr;
FILE* Logging::loggingFile = nullptr;
bool Logging::autoFlush = true;
LogLevel Logging::enabledLogLevel = LogWarn;

void Logging::Setup(void) {
	loggingLock = Platform::MutexCreate();


	char workingDirectory[512];
	Platform::GetWorkingDirectory(workingDirectory, sizeof(workingDirectory));

	char logFilePath[512];
	Platform::snprintf(logFilePath, sizeof(logFilePath), "%s/logs", workingDirectory);
	Platform::MakeDirectory(logFilePath);

	Platform::snprintf(logFilePath, sizeof(logFilePath), "%s/logs/javapluginlog.%lld.log", workingDirectory, Platform::GetMillisAbsolute());
	
	loggingFile = fopen(logFilePath, "w");
	
	if (loggingFile == NULL) {
		fprintf(stderr, "JavaPlugin: Failed to open logging file.\n");
	}
}

const char* Logging::LogLevelName(LogLevel level) {
	switch (level) {
		case LogError:
			return "ERROR";
		case LogInfo:
			return "INFO";
		case LogWarn:
			return "WARN";
		case LogDebug:
			return "DEBUG";
		case LogTrace:
			return "TRACE";
	}

	return "OTHER";
}

bool Logging::Log(LogLevel level, const char* function, const char* format, ...) {
	if (level <= enabledLogLevel && loggingFile != nullptr) {
		char formattedMessage[4096];

		va_list args;
		va_start(args, format);
		Platform::vsnprintf(formattedMessage, sizeof(formattedMessage), format, args);
		va_end(args);

		Platform::MutexLock(loggingLock);
		fprintf(loggingFile, "%lld: %5s - (%s) %s\n", Platform::GetMillisAbsolute(), LogLevelName(level), function, formattedMessage);

		if (autoFlush) {
			fflush(loggingFile);
		}

		Platform::MutexUnlock(loggingLock);
	}

	return level == LogLevel::LogError ? false : true;
}
