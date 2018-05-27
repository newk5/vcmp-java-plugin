#include "main.h"

#define Java_CheckPoint_getSimple(type, methodName, apiName) \
	extern "C" JNIEXPORT type JNICALL Java_com_maxorator_vcmp_java_plugin_integration_placeable_CheckPointImpl_##methodName(JNIEnv* jni, jobject obj) { \
		int checkPointId; \
		if (!core->exc->ThreadCheck(jni)); \
		else if ((checkPointId = core->pools->checkpoints->GetId(jni, obj)) == -1); \
		else { \
			return api->apiName(checkPointId); \
		} \
		return 0; \
	}

#define Java_CheckPoint_setSimple(type, methodName, apiName) \
	extern "C" JNIEXPORT void JNICALL Java_com_maxorator_vcmp_java_plugin_integration_placeable_CheckPointImpl_##methodName(JNIEnv* jni, jobject obj, type value) { \
		int checkPointId; \
		if (!core->exc->ThreadCheck(jni)); \
		else if ((checkPointId = core->pools->checkpoints->GetId(jni, obj)) == -1); \
		else { \
			api->apiName(checkPointId, value); \
		} \
	}

#define Java_CheckPoint_doSimple(methodName, apiName) \
	extern "C" JNIEXPORT void JNICALL Java_com_maxorator_vcmp_java_plugin_integration_placeable_CheckPointImpl_##methodName(JNIEnv* jni, jobject obj) { \
		int checkPointId; \
		if (!core->exc->ThreadCheck(jni)); \
		else if ((checkPointId = core->pools->checkpoints->GetId(jni, obj)) == -1); \
		else { \
			api->apiName(checkPointId); \
		} \
	}

Java_CheckPoint_doSimple(delete, DeleteCheckPoint);

Java_CheckPoint_getSimple(jboolean, isSphere, IsCheckPointSphere);

extern "C" JNIEXPORT jboolean JNICALL Java_com_maxorator_vcmp_java_plugin_integration_placeable_CheckPointImpl_isStreamedForPlayer(JNIEnv* jni, jobject obj, jobject player) {
	int checkPointId, playerId;

	if (!core->exc->ThreadCheck(jni));
	else if ((checkPointId = core->pools->checkpoints->GetId(jni, obj)) == -1);
	else if ((playerId = core->pools->players->GetId(jni, player)) == -1);
	else {
		return api->IsCheckPointStreamedForPlayer(checkPointId, playerId);
	}
	return false;
}

Java_CheckPoint_setSimple(jint, setWorld, SetCheckPointWorld);
Java_CheckPoint_getSimple(jint, getWorld, GetCheckPointWorld);

extern "C" JNIEXPORT void JNICALL Java_com_maxorator_vcmp_java_plugin_integration_placeable_CheckPointImpl_setColour(JNIEnv* jni, jobject obj, jint red, jint green, jint blue, jint alpha) {
	int checkPointId;

	if (!core->exc->ThreadCheck(jni));
	else if ((checkPointId = core->pools->checkpoints->GetId(jni, obj)) == -1);
	else {
		api->SetCheckPointColour(checkPointId, red, green, blue, alpha);
	}
}

extern "C" JNIEXPORT jint JNICALL Java_com_maxorator_vcmp_java_plugin_integration_placeable_CheckPointImpl_getColourHex(JNIEnv* jni, jobject obj) {
	int checkPointId;

	if (!core->exc->ThreadCheck(jni));
	else if ((checkPointId = core->pools->checkpoints->GetId(jni, obj)) == -1);
	else {
		int32_t red = 0, green = 0, blue = 0, alpha = 0;
		api->GetCheckPointColour(checkPointId, &red, &green, &blue, &alpha);
		return (alpha << 24) | (red << 16) | (green << 8) | blue;
	}

	return 0;
}

extern "C" JNIEXPORT void JNICALL Java_com_maxorator_vcmp_java_plugin_integration_placeable_CheckPointImpl_setPosition(JNIEnv* jni, jobject obj, jfloat x, jfloat y, jfloat z) {
	int checkPointId;

	if (!core->exc->ThreadCheck(jni));
	else if ((checkPointId = core->pools->checkpoints->GetId(jni, obj)) == -1);
	else {
		api->SetCheckPointPosition(checkPointId, x, y, z);
	}
}

extern "C" JNIEXPORT jobject JNICALL Java_com_maxorator_vcmp_java_plugin_integration_placeable_CheckPointImpl_getPosition(JNIEnv* jni, jobject obj) {
	int checkPointId;

	if (!core->exc->ThreadCheck(jni));
	else if ((checkPointId = core->pools->checkpoints->GetId(jni, obj)) == -1);
	else {
		float X = 0.0f, Y = 0.0f, Z = 0.0f;
		api->GetCheckPointPosition(checkPointId, &X, &Y, &Z);
		return core->pools->CreateVector(jni, X, Y, Z);
	}

	return nullptr;
}

Java_CheckPoint_setSimple(jfloat, setRadius, SetCheckPointRadius);
Java_CheckPoint_getSimple(jfloat, getRadius, GetCheckPointRadius);

extern "C" JNIEXPORT jobject JNICALL Java_com_maxorator_vcmp_java_plugin_integration_placeable_CheckPointImpl_getOwner(JNIEnv* jni, jobject obj) {
	int checkPointId;

	if (!core->exc->ThreadCheck(jni));
	else if ((checkPointId = core->pools->checkpoints->GetId(jni, obj)) == -1);
	else {
		return core->pools->players->Get(jni, api->GetCheckPointOwner(checkPointId));
	}

	return nullptr;
}
