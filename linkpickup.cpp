#include "main.h"

#define Java_Pickup_getSimple(type, methodName, apiName) \
	extern "C" JNIEXPORT type JNICALL Java_com_maxorator_vcmp_java_plugin_integration_placeable_PickupImpl_##methodName(JNIEnv* jni, jobject obj) { \
		int pickupId; \
		if (!core->exc->ThreadCheck(jni)); \
		else if ((pickupId = core->pools->pickups->GetId(jni, obj)) == -1); \
		else { \
			return api->apiName(pickupId); \
		} \
		return 0; \
	}

#define Java_Pickup_setSimple(type, methodName, apiName) \
	extern "C" JNIEXPORT void JNICALL Java_com_maxorator_vcmp_java_plugin_integration_placeable_PickupImpl_##methodName(JNIEnv* jni, jobject obj, type value) { \
		int pickupId; \
		if (!core->exc->ThreadCheck(jni)); \
		else if ((pickupId = core->pools->pickups->GetId(jni, obj)) == -1); \
		else { \
			api->apiName(pickupId, value); \
		} \
	}

#define Java_Pickup_doSimple(methodName, apiName) \
	extern "C" JNIEXPORT void JNICALL Java_com_maxorator_vcmp_java_plugin_integration_placeable_PickupImpl_##methodName(JNIEnv* jni, jobject obj) { \
		int pickupId; \
		if (!core->exc->ThreadCheck(jni)); \
		else if ((pickupId = core->pools->pickups->GetId(jni, obj)) == -1); \
		else { \
			api->apiName(pickupId); \
		} \
	}

Java_Pickup_doSimple(delete, DeletePickup);

extern "C" JNIEXPORT jboolean JNICALL Java_com_maxorator_vcmp_java_plugin_integration_placeable_PickupImpl_isStreamedForPlayer(JNIEnv* jni, jobject obj, jobject player) {
	int pickupId, playerId;

	if (!core->exc->ThreadCheck(jni));
	else if ((pickupId = core->pools->pickups->GetId(jni, obj)) == -1);
	else if ((playerId = core->pools->players->GetId(jni, player)) == -1);
	else {
		return api->IsPickupStreamedForPlayer(pickupId, playerId);
	}
	return false;
}

Java_Pickup_setSimple(jint, setWorld, SetPickupWorld);
Java_Pickup_getSimple(jint, getWorld, GetPickupWorld);
Java_Pickup_setSimple(jint, setAlpha, SetPickupAlpha);
Java_Pickup_getSimple(jint, getAlpha, GetPickupAlpha);
Java_Pickup_setSimple(jboolean, setAutomatic, SetPickupIsAutomatic);
Java_Pickup_getSimple(jboolean, getAutomatic, IsPickupAutomatic);
Java_Pickup_setSimple(jint, setAutomaticTimer, SetPickupAutoTimer);
Java_Pickup_getSimple(jint, getAutomaticTimer, GetPickupAutoTimer);
Java_Pickup_doSimple(refresh, RefreshPickup);


typedef int(*OPTS_TogglePickupOption) (int pickupId, unsigned int value);

extern "C" JNIEXPORT void JNICALL Java_com_maxorator_vcmp_java_plugin_integration_placeable_PickupImpl_setOption(JNIEnv* jni, jobject obj, jint option, jboolean value) {
	int pickupId;

	if (!core->exc->ThreadCheck(jni));
	else if ((pickupId = core->pools->pickups->GetId(jni, obj)) == -1);
	else {
		api->SetPickupOption(pickupId, (vcmpPickupOption)option, value);
	}
	return;
}

typedef unsigned int(*OPTS_GetPickupOption) (int playerId);

extern "C" JNIEXPORT jboolean JNICALL Java_com_maxorator_vcmp_java_plugin_integration_placeable_PickupImpl_getOption(JNIEnv* jni, jobject obj, jint option) {
	int pickupId;

	if (!core->exc->ThreadCheck(jni));
	else if ((pickupId = core->pools->pickups->GetId(jni, obj)) == -1);
	else {
		return !!api->GetPickupOption(pickupId, (vcmpPickupOption)option);
	}
	return false;
}



extern "C" JNIEXPORT void JNICALL Java_com_maxorator_vcmp_java_plugin_integration_placeable_PickupImpl_setPosition(JNIEnv* jni, jobject obj, jfloat X, jfloat Y, jfloat Z) {
	int pickupId;

	if (!core->exc->ThreadCheck(jni)) return;
	else if ((pickupId = core->pools->pickups->GetId(jni, obj)) == -1) return;

	api->SetPickupPosition(pickupId, X, Y, Z);
}

extern "C" JNIEXPORT jobject JNICALL Java_com_maxorator_vcmp_java_plugin_integration_placeable_PickupImpl_getPosition(JNIEnv* jni, jobject obj) {
	int pickupId;

	if (!core->exc->ThreadCheck(jni)) return nullptr;
	else if ((pickupId = core->pools->pickups->GetId(jni, obj)) == -1) return nullptr;

	float X = 0, Y = 0, Z = 0;
	api->GetPickupPosition(pickupId, &X, &Y, &Z);

	return core->pools->CreateVector(jni, X, Y, Z);
}

Java_Pickup_getSimple(jint, getModel, GetPickupModel);
Java_Pickup_getSimple(jint, getQuantity, GetPickupQuantity);
