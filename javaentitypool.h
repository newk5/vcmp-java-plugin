#pragma once
#include <jni.h>

class JavaEntityPool {
public:
	JavaEntityPool(int size, bool track);
	~JavaEntityPool(void);

	bool BindToClass(const char* className);

	void Register(int id);
	jobject Get(JNIEnv* jni, int id);
	int GetCount(void);
	jobjectArray GetAll(JNIEnv* jni);
	void Delete(int id);

	int GetId(JNIEnv* jni, jobject id);

	jclass klass;

private:
	jobject* handles;
	bool* exists;
	int size;

	jmethodID constructor;
	jfieldID idField;
};
