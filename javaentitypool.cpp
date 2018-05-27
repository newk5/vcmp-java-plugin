#include "javaentitypool.h"
#include "main.h"
#include "logging.h"
#include <string.h>

JavaEntityPool::JavaEntityPool(int size, bool track) {
	this->size = size;

	handles = new jobject[size];

	if (track) {
		exists = new bool[size];
		memset(exists, 0, sizeof(bool) * size);
	}
	else {
		exists = nullptr;
	}

	for (int i = 0; i < size; i++) {
		handles[i] = nullptr;
	}

	klass = nullptr;
	constructor = nullptr;
	idField = nullptr;
}

JavaEntityPool::~JavaEntityPool(void) {
	for (int i = 0; i < size; i++) {
		Delete(i);
	}

	if (klass) {
		core->env->DeleteGlobalRef(klass);
	}

	if (exists != nullptr) {
		delete[] exists;
	}

	delete[] handles;
}

void JavaEntityPool::Register(int id) {
	if (exists != nullptr && id >= 0 && id < size) {
		exists[id] = true;
	}
}

jobject JavaEntityPool::Get(JNIEnv* jni, int id) {
	if (id < 0 || id >= size || (exists != nullptr && !exists[id])) {
		return nullptr;
	}

	if (handles[id] == nullptr) {
		handles[id] = core->ReplaceWithGlobalRef(jni, jni->NewObject(klass, constructor, id));

		if (handles[id] != nullptr) {
			core->jvmti->SetTag(handles[id], id + 1);
		}
		else {
			jni->ExceptionClear();
			Logging::Log(LogError, __FUNCTION__, "Could not create entity instance.");
		}
	}

	return handles[id];
}

int JavaEntityPool::GetCount(void) {
	int count = 0;

	if (exists == nullptr) {
		return 0;
	}

	for (int i = 0; i < size; i++) {
		if (exists[i]) {
			count++;
		}
	}

	return count;
}

jobjectArray JavaEntityPool::GetAll(JNIEnv* jni) {
	int count = GetCount();
	jobjectArray items = jni->NewObjectArray(count, klass, nullptr);

	if (items != nullptr) {
		int fill = 0;

		for (int i = 0; i < size; i++) {
			if (exists[i]) {
				if (fill < count) {
					jni->SetObjectArrayElement(items, fill, Get(jni, i));
				}

				fill++;
			}
		}

		if (jni->ExceptionCheck()) {
			jni->ExceptionClear();
			Logging::Log(LogError, __FUNCTION__, "Failed to set some element in object array.");
		}
	}
	else {
		jni->ExceptionClear();
		Logging::Log(LogError, __FUNCTION__, "Could not create object array of size %d.", items);
	}

	return items;
}

void JavaEntityPool::Delete(int id) {
	if (id >= 0 && id < size) {
		if (handles[id]) {
			core->jvmti->SetTag(handles[id], 0);

			core->env->SetIntField(handles[id], idField, -1);

			core->env->DeleteGlobalRef(handles[id]);
			handles[id] = nullptr;
		}

		if (exists != nullptr) {
			exists[id] = false;
		}
	}
}

int JavaEntityPool::GetId(JNIEnv* jni, jobject object) {
	if (object == nullptr) {
		return -1;
	}

	jlong id = -1;

	core->jvmti->GetTag(object, &id);

	--id;

	if (id < 0 || id >= size) {
		core->exc->ThrowDeadEntityException(jni);
		return -1;
	}

	return (int)id;
}

bool JavaEntityPool::BindToClass(const char* className) {
	klass = (jclass)core->ReplaceWithGlobalRef(core->env, core->env->FindClass(className));
	if (klass == nullptr) {
		return Logging::Log(LogError, __FUNCTION__, "Could not find entity class %s.", className);
	}

	constructor = core->env->GetMethodID(klass, "<init>", "(I)V");
	if (constructor == nullptr) {
		return Logging::Log(LogError, __FUNCTION__, "Could not find class %s constructor.", className);
	}

	idField = core->env->GetFieldID(klass, "id", "I");
	if (idField == nullptr) {
		return Logging::Log(LogError, __FUNCTION__, "Could not find class %s field id.", className);
	}

	Logging::Log(LogInfo, __FUNCTION__, "Loaded entity pool of %s.", className);

	return true;
}
