#include "javastring.h"

JavaString::JavaString(JNIEnv* jni, jstring stringObject) {
	this->jni = jni;
	this->stringObject = stringObject;

	text = jni->GetStringUTFChars(stringObject, nullptr);
	
	if (text == nullptr) {
		useStatic = true;
		text = "";
	}
	else {
		useStatic = false;
	}
}

JavaString::~JavaString(void) {
	if (!useStatic) {
		jni->ReleaseStringUTFChars(stringObject, text);
	}
}
