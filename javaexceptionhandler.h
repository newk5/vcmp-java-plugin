#pragma once
#include <jni.h>
#include <stdint.h>

class JavaExceptionHandler {
public:
	JavaExceptionHandler(void);
	~JavaExceptionHandler(void);

	bool Initialise(void);
	bool PrintExceptions(const char* function);
	bool ThreadCheck(JNIEnv* env);
	bool ThreadCheckStrict(JNIEnv* env);
	bool NullCheck(JNIEnv* jni, jobject object);

	void ThrowNullPointerException(JNIEnv* env);
	void ThrowThreadSafetyException(JNIEnv* env);
	void ThrowDeadEntityException(JNIEnv* env);

	void StartThreadSynced(void);
	void EndThreadSynced(void);

private:

	jclass throwableClass;
	jmethodID throwableCauseMethod;
	jmethodID throwableStackTraceMethod;
	jmethodID throwableStringMethod;
	
	jclass stackTraceElementClass;
	jmethodID stackTraceElementStringMethod;

	jclass threadSafetyExceptionClass;
	jclass deadEntityExceptionClass;
	jclass nullPointerExceptionClass;

	uint64_t threadId;
	uint64_t syncedThreadId;
};
