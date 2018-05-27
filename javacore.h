#pragma once
#include <jni.h>
#include <jvmti.h>
#include "javaeventhandler.h"
#include "javaentitypools.h"
#include "javaexceptionhandler.h"

typedef jint (JNICALL *tfnCreateJavaVM) (JavaVM **pvm, void **penv, void *args);
typedef jint (JNICALL *tfnGetDefaultJavaVMInitArgs) (void *args);

class JavaCore {
public:
	JavaCore(void);
	~JavaCore(void);

	bool Initialise(void);
	bool InitialiseBindings(jclass seedClass);
	bool Reload(void);
	void Unload(void);
	jobject ReplaceWithGlobalRef(JNIEnv* jni, jobject local);
	const char* GetMainClass(void);

	JNIEnv* env;
	jvmtiEnv* jvmti;
	JavaEventHandler* eventHandler;
	JavaEntityPools* pools;
	JavaExceptionHandler* exc;

private:
	static jint JNICALL ReferenceCallback(jvmtiHeapReferenceKind reference_kind, const jvmtiHeapReferenceInfo* reference_info, jlong class_tag,
		jlong referrer_class_tag, jlong size, jlong* tag_ptr, jlong* referrer_tag_ptr, jint length, void* user_data);

	void AnalyzeReferences(void);
	char* TrimString(char* buffer);
	bool SetupModulePath(void);
	bool LoadSettings(void);
	bool LoadLibraryFunctions(void);
	bool SetupJVMArguments(void);
	bool CreateJVM(void);
	bool AttachThreadToJVM(void);
	bool LoadRootClass(void);
	bool SeedClasses(void);
	void RestoreThreadClassLoader(void);
	bool SetupEventHandler(void);
	bool SetupEntityPools(void);
	bool SetupExceptionHandler(void);
	bool SetupJvmti(void);
	void ShutdownBindings(void);

	char m_moduleDirectory[512];
	char m_moduleFileName[256];
	char m_jvmLibPathSetting[512];
	char m_mainClassSetting[256];
	char m_classPathSetting[512];
	char m_memorySetting[16];
	char m_classPathArgument[512];
	char m_libraryPathArgument[512];
	char m_memoryArgument[16];
	char m_debugArgument[128];

	int m_debugPort;

	JavaVMInitArgs m_vmArguments;
	tfnCreateJavaVM m_fnCreateJavaVM;
	tfnGetDefaultJavaVMInitArgs m_fnGetDefaultJavaVMInitArgs;
	JavaVM* m_vm;
	jclass m_rootClass;
	jmethodID m_rootSeedMethod;
	jmethodID m_rootRestoreClassLoaderMethod;
	jobject m_seedClassWeakRef;
	uint64_t m_lastLoadAttempt;
	int m_loadAttemptCount;
	bool m_loadFatalFail;
	bool m_firstBinding;
	void* m_jvmModule;
};
