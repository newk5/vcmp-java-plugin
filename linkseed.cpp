#include "main.h"

extern "C" JNIEXPORT jboolean JNICALL Java_com_maxorator_vcmp_java_plugin_loader_Seed_activate(JNIEnv* jni, jclass klass) {
	return core->InitialiseBindings(klass);
}
