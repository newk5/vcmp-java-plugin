#pragma once

#include <stdio.h>

enum LogLevel {
	LogError,
	LogInfo,
	LogWarn,
	LogDebug,
	LogTrace
};

class Logging {
public:
	static void Setup(void);

	static const char* LogLevelName(LogLevel level);
	static bool Log(LogLevel level, const char* function, const char* format, ...);

	static void* loggingLock;
	static FILE* loggingFile;
	static bool autoFlush;
	static LogLevel enabledLogLevel;
};
