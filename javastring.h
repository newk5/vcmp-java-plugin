#pragma once
#include <jni.h>
#include "javacore.h"

class JavaString {
public:
	JavaString(JNIEnv* jni, jstring stringObject);
	~JavaString(void);

	const char* text;

private:
	
	JNIEnv* jni;
	jstring stringObject;
	bool useStatic;
};
