#include "javaexceptionhandler.h"
#include "main.h"
#include "logging.h"
#include "platform.h"
#include <string>

JavaExceptionHandler::JavaExceptionHandler(void) {
	throwableClass = nullptr;
	stackTraceElementClass = nullptr;
}

JavaExceptionHandler::~JavaExceptionHandler(void) {
	if (throwableClass) {
		core->env->DeleteGlobalRef(throwableClass);
	}

	if (stackTraceElementClass) {
		core->env->DeleteGlobalRef(stackTraceElementClass);
	}

	if (threadSafetyExceptionClass) {
		core->env->DeleteGlobalRef(threadSafetyExceptionClass);
	}

	if (deadEntityExceptionClass) {
		core->env->DeleteGlobalRef(deadEntityExceptionClass);
	}

	if (nullPointerExceptionClass) {
		core->env->DeleteGlobalRef(nullPointerExceptionClass);
	}
}

bool JavaExceptionHandler::Initialise(void) {
	threadId = Platform::GetCurrentThreadId();

	throwableClass = (jclass)core->ReplaceWithGlobalRef(core->env, core->env->FindClass("java/lang/Throwable"));
	stackTraceElementClass = (jclass)core->ReplaceWithGlobalRef(core->env, core->env->FindClass("java/lang/StackTraceElement"));

	if (!throwableClass || !stackTraceElementClass) {
		return Logging::Log(LogError, __FUNCTION__, "Throwable or StackTraceElement class not found.");
	}
	
	throwableCauseMethod = core->env->GetMethodID(throwableClass, "getCause", "()Ljava/lang/Throwable;");
	throwableStackTraceMethod = core->env->GetMethodID(throwableClass, "getStackTrace", "()[Ljava/lang/StackTraceElement;");
	throwableStringMethod = core->env->GetMethodID(throwableClass, "toString", "()Ljava/lang/String;");
	stackTraceElementStringMethod = core->env->GetMethodID(stackTraceElementClass, "toString", "()Ljava/lang/String;");

	if (!throwableCauseMethod || !throwableStackTraceMethod || !throwableStringMethod || !stackTraceElementStringMethod) {
		return Logging::Log(LogError, __FUNCTION__, "Throwable or StackTraceElement methods not found.");
	}

	threadSafetyExceptionClass = (jclass)core->ReplaceWithGlobalRef(core->env, core->env->FindClass("com/maxorator/vcmp/java/plugin/exceptions/ThreadSafetyException"));
	deadEntityExceptionClass = (jclass)core->ReplaceWithGlobalRef(core->env, core->env->FindClass("com/maxorator/vcmp/java/plugin/exceptions/DeadEntityException"));
	nullPointerExceptionClass = (jclass)core->ReplaceWithGlobalRef(core->env, core->env->FindClass("java/lang/NullPointerException"));

	if (!threadSafetyExceptionClass || !deadEntityExceptionClass || !nullPointerExceptionClass) {
		PrintExceptions(__FUNCTION__);
		return Logging::Log(LogError, __FUNCTION__, "Exception classes not found (ThreadSafetyException, DeadEntityException, NullPointerException).");
	}

	return true;
}

// http://stackoverflow.com/questions/10408972/how-to-obtain-a-description-of-a-java-exception-in-c-when-using-jni
void _append_exception_trace_messages(
	JNIEnv&      a_jni_env,
	std::string& a_error_msg,
	jthrowable   a_exception,
	jmethodID    a_mid_throwable_getCause,
	jmethodID    a_mid_throwable_getStackTrace,
	jmethodID    a_mid_throwable_toString,
	jmethodID    a_mid_frame_toString)
{
	// Get the array of StackTraceElements.
	jobjectArray frames =
		(jobjectArray)a_jni_env.CallObjectMethod(
		a_exception,
		a_mid_throwable_getStackTrace);
	jsize frames_length = a_jni_env.GetArrayLength(frames);

	// Add Throwable.toString() before descending
	// stack trace messages.
	if (0 != frames)
	{
		jstring msg_obj =
			(jstring)a_jni_env.CallObjectMethod(a_exception,
			a_mid_throwable_toString);
		const char* msg_str = a_jni_env.GetStringUTFChars(msg_obj, 0);

		// If this is not the top-of-the-trace then
		// this is a cause.
		if (!a_error_msg.empty())
		{
			a_error_msg += "\nCaused by: ";
			a_error_msg += msg_str;
		}
		else
		{
			a_error_msg = msg_str;
		}

		a_jni_env.ReleaseStringUTFChars(msg_obj, msg_str);
		a_jni_env.DeleteLocalRef(msg_obj);
	}

	// Append stack trace messages if there are any.
	if (frames_length > 0)
	{
		jsize i = 0;
		for (i = 0; i < frames_length; i++)
		{
			// Get the string returned from the 'toString()'
			// method of the next frame and append it to
			// the error message.
			jobject frame = a_jni_env.GetObjectArrayElement(frames, i);
			jstring msg_obj =
				(jstring)a_jni_env.CallObjectMethod(frame,
				a_mid_frame_toString);

			const char* msg_str = a_jni_env.GetStringUTFChars(msg_obj, 0);

			a_error_msg += "\n    ";
			a_error_msg += msg_str;

			a_jni_env.ReleaseStringUTFChars(msg_obj, msg_str);
			a_jni_env.DeleteLocalRef(msg_obj);
			a_jni_env.DeleteLocalRef(frame);
		}
	}

	// If 'a_exception' has a cause then append the
	// stack trace messages from the cause.
	if (0 != frames)
	{
		jthrowable cause =
			(jthrowable)a_jni_env.CallObjectMethod(
			a_exception,
			a_mid_throwable_getCause);
		if (0 != cause)
		{
			_append_exception_trace_messages(a_jni_env,
				a_error_msg,
				cause,
				a_mid_throwable_getCause,
				a_mid_throwable_getStackTrace,
				a_mid_throwable_toString,
				a_mid_frame_toString);

			a_jni_env.DeleteLocalRef(cause);
		}

		a_jni_env.DeleteLocalRef(frames);
	}
}

bool JavaExceptionHandler::PrintExceptions(const char* function) {
	if (core->env->ExceptionCheck()) {
		jthrowable exception = core->env->ExceptionOccurred();
		std::string exceptionData;
		
		_append_exception_trace_messages(*core->env, exceptionData, exception, throwableCauseMethod, throwableStackTraceMethod, throwableStringMethod, stackTraceElementStringMethod);

		Logging::Log(LogError, function, "Exception in Java code\n%s", exceptionData.c_str());

		core->env->DeleteLocalRef(exception);
		core->env->ExceptionClear();

		return true;
	}

	return false;
}

bool JavaExceptionHandler::ThreadCheck(JNIEnv* jni) {
	uint64_t currentThread = Platform::GetCurrentThreadId();
	if (currentThread == threadId || currentThread == syncedThreadId) {
		return true;
	}

	ThrowThreadSafetyException(jni);

	return false;
}

bool JavaExceptionHandler::ThreadCheckStrict(JNIEnv* jni) {
	uint64_t currentThread = Platform::GetCurrentThreadId();
	if (currentThread == threadId) {
		return true;
	}

	ThrowThreadSafetyException(jni);

	return false;
}

bool JavaExceptionHandler::NullCheck(JNIEnv* jni, jobject object) {
	if (object) {
		return true;
	}

	ThrowNullPointerException(jni);

	return false;
}

void JavaExceptionHandler::ThrowNullPointerException(JNIEnv* jni) {
	jni->ThrowNew(nullPointerExceptionClass, "null argument not allowed here.");
}

void JavaExceptionHandler::ThrowThreadSafetyException(JNIEnv* jni) {
	jni->ThrowNew(threadSafetyExceptionClass, "Trying to access API from a different thread.");
}

void JavaExceptionHandler::ThrowDeadEntityException(JNIEnv* jni) {
	jni->ThrowNew(deadEntityExceptionClass, "Trying to access an entity that no longer exists.");
}

void JavaExceptionHandler::StartThreadSynced(void) {
	syncedThreadId = Platform::GetCurrentThreadId();
}

void JavaExceptionHandler::EndThreadSynced(void) {
	syncedThreadId = 0;
}
