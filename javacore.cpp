#include "main.h"
#include "javacore.h"
#include "logging.h"
#include "platform.h"
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

JavaCore::JavaCore(void) {
	env = nullptr;
	jvmti = nullptr;
	eventHandler = nullptr;
	pools = nullptr;
	exc = nullptr;

	m_jvmModule = nullptr;
	m_vm = nullptr;
	m_fnCreateJavaVM = nullptr;
	m_fnGetDefaultJavaVMInitArgs = nullptr;
	m_lastLoadAttempt = 0;
	m_loadAttemptCount = 0;
	m_seedClassWeakRef = nullptr;
	m_loadFatalFail = false;
	m_firstBinding = true;
	m_rootSeedMethod = nullptr;
	m_rootRestoreClassLoaderMethod = nullptr;

	m_moduleDirectory[0] = '\0';
	m_moduleFileName[0] = '\0';
	m_jvmLibPathSetting[0] = '\0';
	m_mainClassSetting[0] = '\0';
	m_classPathSetting[0] = '\0';
	m_memorySetting[0] = '\0';
	m_classPathArgument[0] = '\0';
	m_libraryPathArgument[0] = '\0';
	m_debugArgument[0] = '\0';

	m_debugPort = 0;

	Platform::strncpy(m_memoryArgument, "80m", sizeof(m_memoryArgument));

	memset(&m_vmArguments, 0, sizeof(m_vmArguments));
}

JavaCore::~JavaCore(void) {
	if (m_vmArguments.options) {
		delete[] m_vmArguments.options;
	}

	if (eventHandler) {
		delete eventHandler;
	}

	if (pools) {
		delete pools;
	}

	if (exc) {
		delete exc;
	}

	if (m_vm) {
		int result = m_vm->DestroyJavaVM();
		Logging::Log(LogInfo, __FUNCTION__, "Destroying VM returned: %d", result);
	}
}

bool JavaCore::SetupModulePath(void) {
	char moduleLocation[512];

	if (!Platform::GetCurrentModulePath(moduleLocation, sizeof(moduleLocation))) {
		Logging::Log(LogError, __FUNCTION__, "Could not get module location");
		return false;
	}

	char* lastSlash = strrchr(moduleLocation, '/');
	char* lastBackwardSlash = strrchr(moduleLocation, '\\');

	if (lastBackwardSlash > lastSlash) {
		lastSlash = lastBackwardSlash;
	}

	if (lastSlash == nullptr) {
		Logging::Log(LogError, __FUNCTION__, "Invalid module location");
		return false;
	}

	lastSlash[0] = '\0';

	Platform::strncpy(m_moduleDirectory, moduleLocation, sizeof(m_moduleDirectory));
	Platform::strncpy(m_moduleFileName, &lastSlash[1], sizeof(m_moduleFileName));

	return true;
}

char* JavaCore::TrimString(char* buffer) {
	char *start = buffer, *end = buffer + strlen(buffer);

	while (end > start && isspace(end[-1])) {
		*(--end) = '\0';
	}

	while (end > start && isspace(start[0])) {
		*(start++) = '\0';
	}

	return start;
}

bool JavaCore::LoadSettings(void) {
	if (!SetupModulePath()) {
		return false;
	}

	FILE* settingsFile = fopen("javaplugin.properties", "r");
	if (settingsFile == nullptr) {
		Logging::Log(LogError, __FUNCTION__, "javaplugin.properties file not present.");
		return false;
	}

	char line[512];

	while (fgets(line, sizeof(line), settingsFile) != nullptr) {
		char* splitter = strchr(line, '=');

		if (splitter != nullptr) {
			*splitter = '\0';

			char* name = TrimString(line);
			char* value = TrimString(&splitter[1]);

			if (strcmp(name, "jvmLibPath") == 0) {
				Platform::strncpy(m_jvmLibPathSetting, value, sizeof(m_jvmLibPathSetting));
			}
			else if (strcmp(name, "mainEventsClass") == 0) {
				Platform::strncpy(m_mainClassSetting, value, sizeof(m_mainClassSetting));
				
				char* dotPosition;

				while ((dotPosition = strchr(m_mainClassSetting, '.')) != nullptr) {
					*dotPosition = '/';
				}
			}
			else if (strcmp(name, "classPath") == 0) {
				Platform::strncpy(m_classPathSetting, value, sizeof(m_classPathSetting));
			}
			else if (strcmp(name, "maxMemory") == 0) {
				Platform::strncpy(m_memorySetting, value, sizeof(m_memorySetting));
			}
			else if (strcmp(name, "debugPort") == 0) {
				m_debugPort = (int)strtol(value, nullptr, 10);
			}
		}
	}

	fclose(settingsFile);

	if (m_jvmLibPathSetting[0] == '\0') {
		return Logging::Log(LogError, __FUNCTION__, "jvmLibPath setting missing or empty");
	}
	else if (m_mainClassSetting[0] == '\0') {
		return Logging::Log(LogError, __FUNCTION__, "mainEventsClass setting missing or empty");
	}
	else if (m_classPathSetting[0] == '\0') {
		return Logging::Log(LogError, __FUNCTION__, "classPath setting missing or empty");
	}
	else if (m_memorySetting[0] == '\0') {
		return Logging::Log(LogError, __FUNCTION__, "maxMemory setting missing or empty");
	}

	return true;
}

bool JavaCore::LoadLibraryFunctions(void) {
	if ((m_jvmModule = Platform::LoadModule(m_jvmLibPathSetting)) == nullptr) {
		return false;
	}

	Logging::Log(LogInfo, __FUNCTION__, "Java VM library loaded.");

	m_fnCreateJavaVM = (tfnCreateJavaVM)Platform::LoadFunction(m_jvmModule, "JNI_CreateJavaVM");
	m_fnGetDefaultJavaVMInitArgs = (tfnGetDefaultJavaVMInitArgs)Platform::LoadFunction(m_jvmModule, "JNI_GetDefaultJavaVMInitArgs");

	if (m_fnCreateJavaVM == nullptr || m_fnGetDefaultJavaVMInitArgs == nullptr) {
		return false;
	}

	Logging::Log(LogInfo, __FUNCTION__, "Java VM library functions loaded.");

	return true;
}

bool JavaCore::SetupJVMArguments(void) {
	m_vmArguments.version = JNI_VERSION_1_6;

	jint result = m_fnGetDefaultJavaVMInitArgs(&m_vmArguments);
	if (result != JNI_OK) {
		return Logging::Log(LogError, __FUNCTION__, "JNI_GetDefaultJavaVMInitArgs failed with error: %d", result);
	}
	m_vmArguments.ignoreUnrecognized = false;

	Platform::snprintf(m_classPathArgument, sizeof(m_classPathArgument), "-Djava.class.path=%s", m_classPathSetting);
	Platform::snprintf(m_libraryPathArgument, sizeof(m_libraryPathArgument), "-Djava.library.path=%s", m_moduleDirectory);
	Platform::snprintf(m_memoryArgument, sizeof(m_memoryArgument), "-Xmx%s", m_memorySetting);
	Platform::snprintf(m_debugArgument, sizeof(m_debugArgument), "-agentlib:jdwp=transport=dt_socket,server=y,suspend=y,address=%d", m_debugPort);

	JavaVMOption* options = new JavaVMOption[4];
	options[0].optionString = m_classPathArgument;
	options[0].extraInfo = nullptr;
	options[1].optionString = m_libraryPathArgument;
	options[1].extraInfo = nullptr;
	options[2].optionString = m_memoryArgument;
	options[2].extraInfo = nullptr;
	options[3].optionString = m_debugArgument;
	options[3].extraInfo = nullptr;

	m_vmArguments.nOptions = m_debugPort == 0 ? 3 : 4;
	m_vmArguments.options = options;

	Logging::Log(LogInfo, __FUNCTION__, "Java VM settings initialized.");

	return true;
}

bool JavaCore::CreateJVM(void) {
	JNIEnv* tempEnv;
	jint result = m_fnCreateJavaVM(&m_vm, (void**)&tempEnv, &m_vmArguments);
	if (result != JNI_OK) {
		return Logging::Log(LogError, __FUNCTION__, "JNI_CreateJavaVM failed with error: %d", result);
	}

	Logging::Log(LogInfo, __FUNCTION__, "Java VM created.");

	return true;
}

bool JavaCore::AttachThreadToJVM(void) {
	JavaVMAttachArgs attachArgs;
	attachArgs.group = NULL;
	attachArgs.name = (char*)"VC:MP core thread";
	attachArgs.version = JNI_VERSION_1_8;

	jint result = m_vm->AttachCurrentThread((void**)&env, &attachArgs);
	if (result != JNI_OK) {
		return Logging::Log(LogError, __FUNCTION__, "vm->AttachCurrentThread failed with error: %d", result);
	}

	Logging::Log(LogInfo, __FUNCTION__, "Server thread attached to VM.");

	return true;
}

bool JavaCore::LoadRootClass(void) {
	m_rootClass = (jclass)ReplaceWithGlobalRef(env, env->FindClass("com/maxorator/vcmp/java/plugin/loader/Root"));
	if (!m_rootClass) {
		exc->PrintExceptions(__FUNCTION__);

		return Logging::Log(LogError, __FUNCTION__, "Root class not found.");
	}

	m_rootSeedMethod = env->GetStaticMethodID(m_rootClass, "seed", "(ILjava/lang/String;)Z");
	m_rootRestoreClassLoaderMethod = env->GetStaticMethodID(m_rootClass, "restoreClassLoader", "()V");

	if (!m_rootSeedMethod || !m_rootRestoreClassLoaderMethod) {
		exc->PrintExceptions(__FUNCTION__);

		return Logging::Log(LogError, __FUNCTION__, "Root.seed method not found.");
	}

	return true;
}

bool JavaCore::SeedClasses(void) {
	jstring libName = env->NewStringUTF(m_moduleFileName);

	Logging::Log(LogInfo, __FUNCTION__, "Starting script context.");

	bool returnValue = env->CallStaticBooleanMethod(m_rootClass, m_rootSeedMethod, JAVA_PLUGIN_VERSION >> 8, libName) ? true : false;

	env->DeleteLocalRef(libName);
	
	if (exc->PrintExceptions(__FUNCTION__)) {
		return false;
	}

	return returnValue;
}

void JavaCore::RestoreThreadClassLoader(void) {
	env->CallStaticVoidMethod(m_rootClass, m_rootRestoreClassLoaderMethod);
	exc->PrintExceptions(__FUNCTION__);
}

bool JavaCore::SetupEventHandler(void) {
	eventHandler = new JavaEventHandler();
	return eventHandler->BindToClasses(m_firstBinding, 0x7FFFFFFFFFFFFFFFLL, nullptr);
}

bool JavaCore::SetupEntityPools(void) {
	pools = new JavaEntityPools();
	return pools->Initialise(m_firstBinding);
}

bool JavaCore::SetupExceptionHandler(void) {
	exc = new JavaExceptionHandler();
	return exc->Initialise();
}

bool JavaCore::SetupJvmti(void) {
	if (m_vm->GetEnv((void**)&jvmti, JVMTI_VERSION_1_2) != JNI_OK) {
		return Logging::Log(LogError, __FUNCTION__, "Getting JVMTI environment failed.");
	}

	jvmtiCapabilities potentialCaps, requestedCaps;
	
	if (jvmti->GetPotentialCapabilities(&potentialCaps) != JVMTI_ERROR_NONE) {
		return Logging::Log(LogError, __FUNCTION__, "Getting JVMTI potential capabilities failed.");
	}

	memset(&requestedCaps, 0, sizeof(requestedCaps));

	requestedCaps.can_tag_objects = 1;

	if (jvmti->AddCapabilities(&requestedCaps) != JVMTI_ERROR_NONE) {
		return Logging::Log(LogError, __FUNCTION__, "Adding JVMTI capabilities failed.");
	}

	return true;
}

bool JavaCore::Initialise(void) {
	if (!LoadSettings()) {
		return Logging::Log(LogError, __FUNCTION__, "Loading settings failed.");
	} else if (!LoadLibraryFunctions()) {
		return Logging::Log(LogError, __FUNCTION__, "Loading JVM library functions failed.");
	} else if (!SetupJVMArguments()) {
		return Logging::Log(LogError, __FUNCTION__, "Setting up JVM arguments failed.");
	} else if (!CreateJVM()) {
		return Logging::Log(LogError, __FUNCTION__, "Creating JVM failed.");
	} else if (!SetupJvmti()) {
		return Logging::Log(LogError, __FUNCTION__, "Setting up JVMTI failed.");
	} else if (!AttachThreadToJVM()) {
		return Logging::Log(LogError, __FUNCTION__, "Attaching current thread to JVM failed.");
	} else if (!SetupExceptionHandler()) {
		return Logging::Log(LogError, __FUNCTION__, "Setting up exception handler failed.");
	} else if (!LoadRootClass()) {
		return Logging::Log(LogError, __FUNCTION__, "Loading root class failed.");
	} else if (!SeedClasses()) {
		return Logging::Log(LogError, __FUNCTION__, "Seeding classes failed.");
	}

	return true;
}

bool JavaCore::InitialiseBindings(jclass seedClass) {
	m_seedClassWeakRef = env->NewWeakGlobalRef(seedClass);

	if (!SetupEventHandler()) {
		return Logging::Log(LogError, __FUNCTION__, "Setting up event handler failed.");
	}
	else if (!SetupEntityPools()) {
		return Logging::Log(LogError, __FUNCTION__, "Setting up entity pools failed.");
	}

	m_firstBinding = false;

	return true;
}

void JavaCore::ShutdownBindings(void) {
	if (eventHandler) {
		delete eventHandler;
		eventHandler = nullptr;
	}

	if (pools) {
		delete pools;
		pools = nullptr;
	}

	RestoreThreadClassLoader();
}

void JavaCore::Unload(void) {
	ShutdownBindings();

	m_lastLoadAttempt = 0;
	m_loadAttemptCount = 0;
}

jint JNICALL JavaCore::ReferenceCallback(jvmtiHeapReferenceKind reference_kind, const jvmtiHeapReferenceInfo* reference_info, jlong class_tag,
	jlong referrer_class_tag, jlong size, jlong* tag_ptr, jlong* referrer_tag_ptr, jint length, void* user_data) {
	
	if (reference_kind == JVMTI_HEAP_REFERENCE_JNI_GLOBAL) {
		*tag_ptr = 1000;
	}
	else if (reference_kind == JVMTI_HEAP_REFERENCE_JNI_LOCAL) {
		*tag_ptr = 1001;
	}
	else if (reference_kind == JVMTI_HEAP_REFERENCE_STACK_LOCAL) {
		*tag_ptr = 1002;
	}

	return 0;
}

void JavaCore::AnalyzeReferences(void) {
	jvmtiHeapCallbacks callbacks;
	memset(&callbacks, 0, sizeof(callbacks));
	callbacks.heap_reference_callback = ReferenceCallback;

	if (jvmti->FollowReferences(0, nullptr, nullptr, &callbacks, nullptr) != JVMTI_ERROR_NONE) {
		Logging::Log(LogError, __FUNCTION__, "Failed to follow references.");
		return;
	}

	const char* spaceNames[3] = { "jni_global", "jni_local", "stack_local" };
	jlong tags[3] = { 1000, 1001, 1002 };
	jint resultCount;
	jlong* resultTags;
	jobject* resultObjects;

	if (jvmti->GetObjectsWithTags(3, tags, &resultCount, &resultObjects, &resultTags) != JVMTI_ERROR_NONE) {
		Logging::Log(LogError, __FUNCTION__, "Failed to search for objects by tag.");
		return;
	}

	jobject globalRef = env->NewGlobalRef(m_seedClassWeakRef);

	if (globalRef != nullptr) {
		jobject seedClassLoader = nullptr;
		
		if (jvmti->GetClassLoader((jclass)globalRef, &seedClassLoader) == JVMTI_ERROR_NONE) {
			for (int i = 0; i < resultCount; i++) {
				jclass klass = (jclass)resultObjects[i];

				for (int j = 0; j < 2; j++) {
					jobject loader = nullptr;

					if (jvmti->GetClassLoader(klass, &loader) == JVMTI_ERROR_NONE) {
						if (env->IsSameObject(loader, seedClassLoader)) {
							char* klass_signature;

							if (jvmti->GetClassSignature(klass, &klass_signature, nullptr) == JVMTI_ERROR_NONE) {
								Logging::Log(LogInfo, __FUNCTION__, "Blocking reference of type %s to %s.", spaceNames[resultTags[i] - 1000], klass_signature);

								jvmti->Deallocate((unsigned char*)klass_signature);
							}
						}

						env->DeleteLocalRef(loader);
					}
					else {
						klass = env->GetObjectClass(klass);
					}
				}

				if (klass != resultObjects[i]) {
					env->DeleteLocalRef(klass);
				}
			}

			env->DeleteLocalRef(seedClassLoader);
		}

		env->DeleteGlobalRef(globalRef);
	}

	for (int i = 0; i < resultCount; i++) {
		env->DeleteLocalRef(resultObjects[i]);
	}

	jvmti->Deallocate((unsigned char*)resultTags);
	jvmti->Deallocate((unsigned char*)resultObjects);
}

bool JavaCore::Reload(void) {
	uint64_t currentTicks = Platform::GetMillisTicks();

	if (m_loadFatalFail || m_lastLoadAttempt + 500 >= currentTicks) {
		return false;
	}

	m_lastLoadAttempt = currentTicks;

	if (jvmti->ForceGarbageCollection() != JVMTI_ERROR_NONE) {
		m_loadFatalFail = true;

		return Logging::Log(LogError, __FUNCTION__, "Failed to force garbage collection.");
	}

	if (m_seedClassWeakRef != nullptr) {
		jobject globalRef = env->NewGlobalRef(m_seedClassWeakRef);

		if (globalRef != nullptr) {
			Logging::Log(LogWarn, __FUNCTION__, "Old classes still in use, waiting another second.");

			env->DeleteGlobalRef(globalRef);

			if (++m_loadAttemptCount == 10) {
				AnalyzeReferences();
			}

			return false;
		}

		env->DeleteWeakGlobalRef(m_seedClassWeakRef);
		m_seedClassWeakRef = nullptr;
	}

	if (!SeedClasses()) {
		Logging::Log(LogError, __FUNCTION__, "Failed to reload classes.");

		m_loadFatalFail = true;
		return false;
	}

	Logging::Log(LogInfo, __FUNCTION__, "Scripts reloaded.");

	return true;
}

jobject JavaCore::ReplaceWithGlobalRef(JNIEnv* jni, jobject local) {
	if (local == nullptr) {
		return nullptr;
	}

	jobject global = env->NewGlobalRef(local);
	env->DeleteLocalRef(local);
	return global;
}

const char* JavaCore::GetMainClass(void) {
	return m_mainClassSetting;
}
