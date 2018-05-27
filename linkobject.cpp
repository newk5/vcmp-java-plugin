#include "main.h"

#define Java_GameObject_getSimple(type, methodName, apiName) \
	extern "C" JNIEXPORT type JNICALL Java_com_maxorator_vcmp_java_plugin_integration_placeable_GameObjectImpl_##methodName(JNIEnv* jni, jobject obj) { \
		int objectId; \
		if (!core->exc->ThreadCheck(jni)); \
		else if ((objectId = core->pools->objects->GetId(jni, obj)) == -1); \
		else { \
			return api->apiName(objectId); \
		} \
		return 0; \
	}

#define Java_GameObject_setSimple(type, methodName, apiName) \
	extern "C" JNIEXPORT void JNICALL Java_com_maxorator_vcmp_java_plugin_integration_placeable_GameObjectImpl_##methodName(JNIEnv* jni, jobject obj, type value) { \
		int objectId; \
		if (!core->exc->ThreadCheck(jni)); \
		else if ((objectId = core->pools->objects->GetId(jni, obj)) == -1); \
		else { \
			api->apiName(objectId, value); \
		} \
	}

#define Java_GameObject_doSimple(methodName, apiName) \
	extern "C" JNIEXPORT void JNICALL Java_com_maxorator_vcmp_java_plugin_integration_placeable_GameObjectImpl_##methodName(JNIEnv* jni, jobject obj) { \
		int objectId; \
		if (!core->exc->ThreadCheck(jni)); \
		else if ((objectId = core->pools->objects->GetId(jni, obj)) == -1); \
		else { \
			api->apiName(objectId); \
		} \
	}

Java_GameObject_doSimple(delete, DeleteObject);

extern "C" JNIEXPORT jboolean JNICALL Java_com_maxorator_vcmp_java_plugin_integration_placeable_GameObjectImpl_isStreamedForPlayer(JNIEnv* jni, jobject obj, jobject player) {
	int objectId, playerId;

	if (!core->exc->ThreadCheck(jni));
	else if ((objectId = core->pools->objects->GetId(jni, obj)) == -1);
	else if ((playerId = core->pools->players->GetId(jni, player)) == -1);
	else {
		return api->IsObjectStreamedForPlayer(objectId, playerId);
	}
	return false;
}

Java_GameObject_getSimple(jint, getModel, GetObjectModel);
Java_GameObject_setSimple(jint, setWorld, SetObjectWorld);
Java_GameObject_getSimple(jint, getWorld, GetObjectWorld);

extern "C" JNIEXPORT void JNICALL Java_com_maxorator_vcmp_java_plugin_integration_placeable_GameObjectImpl_setAlpha(JNIEnv* jni, jobject obj, jint value, jint fadeTime) {
	int objectId;

	if (!core->exc->ThreadCheck(jni));
	else if ((objectId = core->pools->objects->GetId(jni, obj)) == -1);
	else {
		api->SetObjectAlpha(objectId, value, fadeTime);
	}
}

Java_GameObject_getSimple(jint, getAlpha, GetObjectAlpha);

extern "C" JNIEXPORT void JNICALL Java_com_maxorator_vcmp_java_plugin_integration_placeable_GameObjectImpl_moveTo(JNIEnv* jni, jobject obj, jfloat X, jfloat Y, jfloat Z, jint millis) {
	int objectId;

	if (!core->exc->ThreadCheck(jni)) return;
	else if ((objectId = core->pools->objects->GetId(jni, obj)) == -1) return;

	api->MoveObjectTo(objectId, X, Y, Z, millis);
}

extern "C" JNIEXPORT void JNICALL Java_com_maxorator_vcmp_java_plugin_integration_placeable_GameObjectImpl_moveBy(JNIEnv* jni, jobject obj, jfloat X, jfloat Y, jfloat Z, jint millis) {
	int objectId;

	if (!core->exc->ThreadCheck(jni)) return;
	else if ((objectId = core->pools->objects->GetId(jni, obj)) == -1) return;

	api->MoveObjectBy(objectId, X, Y, Z, millis);
}

extern "C" JNIEXPORT void JNICALL Java_com_maxorator_vcmp_java_plugin_integration_placeable_GameObjectImpl_setPosition(JNIEnv* jni, jobject obj, jfloat X, jfloat Y, jfloat Z) {
	int objectId;

	if (!core->exc->ThreadCheck(jni)) return;
	else if ((objectId = core->pools->objects->GetId(jni, obj)) == -1) return;

	api->SetObjectPosition(objectId, X, Y, Z);
}

extern "C" JNIEXPORT jobject JNICALL Java_com_maxorator_vcmp_java_plugin_integration_placeable_GameObjectImpl_getPosition(JNIEnv* jni, jobject obj) {
	int objectId;

	if (!core->exc->ThreadCheck(jni)) return nullptr;
	else if ((objectId = core->pools->objects->GetId(jni, obj)) == -1) return nullptr;

	float X = 0, Y = 0, Z = 0;
	api->GetObjectPosition(objectId, &X, &Y, &Z);

	return core->pools->CreateVector(jni, X, Y, Z);
}

extern "C" JNIEXPORT void JNICALL Java_com_maxorator_vcmp_java_plugin_integration_placeable_GameObjectImpl_rotateTo(JNIEnv* jni, jobject obj, jfloat X, jfloat Y, jfloat Z, jfloat W, jint millis) {
	int objectId;

	if (!core->exc->ThreadCheck(jni)) return;
	else if ((objectId = core->pools->objects->GetId(jni, obj)) == -1) return;

	api->RotateObjectTo(objectId, X, Y, Z, W, millis);
}

extern "C" JNIEXPORT void JNICALL Java_com_maxorator_vcmp_java_plugin_integration_placeable_GameObjectImpl_rotateToEuler(JNIEnv* jni, jobject obj, jfloat X, jfloat Y, jfloat Z, jint millis) {
	int objectId;

	if (!core->exc->ThreadCheck(jni)) return;
	else if ((objectId = core->pools->objects->GetId(jni, obj)) == -1) return;

	api->RotateObjectToEuler(objectId, X, Y, Z, millis);
}

extern "C" JNIEXPORT void JNICALL Java_com_maxorator_vcmp_java_plugin_integration_placeable_GameObjectImpl_rotateBy(JNIEnv* jni, jobject obj, jfloat X, jfloat Y, jfloat Z, jfloat W, jint millis) {
	int objectId;

	if (!core->exc->ThreadCheck(jni)) return;
	else if ((objectId = core->pools->objects->GetId(jni, obj)) == -1) return;

	api->RotateObjectBy(objectId, X, Y, Z, W, millis);
}

extern "C" JNIEXPORT void JNICALL Java_com_maxorator_vcmp_java_plugin_integration_placeable_GameObjectImpl_rotateByEuler(JNIEnv* jni, jobject obj, jfloat X, jfloat Y, jfloat Z, jint millis) {
	int objectId;

	if (!core->exc->ThreadCheck(jni)) return;
	else if ((objectId = core->pools->objects->GetId(jni, obj)) == -1) return;

	api->RotateObjectByEuler(objectId, X, Y, Z, millis);
}

extern "C" JNIEXPORT jobject JNICALL Java_com_maxorator_vcmp_java_plugin_integration_placeable_GameObjectImpl_getRotation(JNIEnv* jni, jobject obj) {
	int objectId;

	if (!core->exc->ThreadCheck(jni)) return nullptr;
	else if ((objectId = core->pools->objects->GetId(jni, obj)) == -1) return nullptr;

	float X = 0, Y = 0, Z = 0, W = 0;
	api->GetObjectRotation(objectId, &X, &Y, &Z, &W);

	return core->pools->CreateQuaternion(jni, X, Y, Z, W);
}

extern "C" JNIEXPORT jobject JNICALL Java_com_maxorator_vcmp_java_plugin_integration_placeable_GameObjectImpl_getRotationEuler(JNIEnv* jni, jobject obj) {
	int objectId;

	if (!core->exc->ThreadCheck(jni)) return nullptr;
	else if ((objectId = core->pools->objects->GetId(jni, obj)) == -1) return nullptr;

	float X = 0, Y = 0, Z = 0;
	api->GetObjectRotationEuler(objectId, &X, &Y, &Z);

	return core->pools->CreateVector(jni, X, Y, Z);
}

Java_GameObject_setSimple(jboolean, setShotReportEnabled, SetObjectShotReportEnabled);
Java_GameObject_getSimple(jboolean, isShotReportEnabled, IsObjectShotReportEnabled);
Java_GameObject_setSimple(jboolean, setTouchedReportEnabled, SetObjectTouchedReportEnabled);
Java_GameObject_getSimple(jboolean, isTouchedReportEnabled, IsObjectTouchedReportEnabled);
