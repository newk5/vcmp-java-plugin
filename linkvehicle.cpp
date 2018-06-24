#include "main.h"

#define Java_Vehicle_getSimple(type, methodName, apiName) \
	extern "C" JNIEXPORT type JNICALL Java_com_maxorator_vcmp_java_plugin_integration_vehicle_VehicleImpl_##methodName(JNIEnv* jni, jobject obj) { \
		int vehicleId; \
		if (!core->exc->ThreadCheck(jni)); \
		else if ((vehicleId = core->pools->vehicles->GetId(jni, obj)) == -1); \
		else { \
			return api->apiName(vehicleId); \
		} \
		return 0; \
	}

#define Java_Vehicle_setSimple(type, methodName, apiName) \
	extern "C" JNIEXPORT void JNICALL Java_com_maxorator_vcmp_java_plugin_integration_vehicle_VehicleImpl_##methodName(JNIEnv* jni, jobject obj, type value) { \
		int vehicleId; \
		if (!core->exc->ThreadCheck(jni)); \
		else if ((vehicleId = core->pools->vehicles->GetId(jni, obj)) == -1); \
		else { \
			api->apiName(vehicleId, value); \
		} \
	}

#define Java_Vehicle_doSimple(methodName, apiName) \
	extern "C" JNIEXPORT void JNICALL Java_com_maxorator_vcmp_java_plugin_integration_vehicle_VehicleImpl_##methodName(JNIEnv* jni, jobject obj) { \
		int vehicleId; \
		if (!core->exc->ThreadCheck(jni)); \
		else if ((vehicleId = core->pools->vehicles->GetId(jni, obj)) == -1); \
		else { \
			api->apiName(vehicleId); \
		} \
	}

Java_Vehicle_doSimple(delete, DeleteVehicle);

extern "C" JNIEXPORT jobject JNICALL Java_com_maxorator_vcmp_java_plugin_integration_vehicle_VehicleImpl_getSyncController(JNIEnv* jni, jobject obj) {
	int vehicleId;

	if (!core->exc->ThreadCheck(jni));
	else if ((vehicleId = core->pools->vehicles->GetId(jni, obj)) == -1);
	else {
		return core->pools->players->Get(jni, api->GetVehicleSyncSource(vehicleId));
	}

	return nullptr;
}

Java_Vehicle_getSimple(jint, getSyncReasonOrdinal, GetVehicleSyncType);

extern "C" JNIEXPORT jboolean JNICALL Java_com_maxorator_vcmp_java_plugin_integration_vehicle_VehicleImpl_isStreamedForPlayer(JNIEnv* jni, jobject obj, jobject player) {
	int vehicleId, playerId;

	if (!core->exc->ThreadCheck(jni));
	else if ((vehicleId = core->pools->vehicles->GetId(jni, obj)) == -1);
	else if ((playerId = core->pools->players->GetId(jni, player)) == -1);
	else {
		return api->IsVehicleStreamedForPlayer(vehicleId, playerId);
	}
	return false;
}

Java_Vehicle_setSimple(jint, setWorld, SetVehicleWorld);
Java_Vehicle_getSimple(jint, getWorld, GetVehicleWorld);
Java_Vehicle_getSimple(jint, getModel, GetVehicleModel);

extern "C" JNIEXPORT jobject JNICALL Java_com_maxorator_vcmp_java_plugin_integration_vehicle_VehicleImpl_getOccupant(JNIEnv* jni, jobject obj, jint slot) {
	int vehicleId;

	if (!core->exc->ThreadCheck(jni));
	else if ((vehicleId = core->pools->vehicles->GetId(jni, obj)) == -1);
	else {
		return core->pools->players->Get(jni, api->GetVehicleOccupant(vehicleId, slot));
	}
	return nullptr;
}

Java_Vehicle_doSimple(respawn, RespawnVehicle);
Java_Vehicle_setSimple(jint, setImmunityFlags, SetVehicleImmunityFlags);
Java_Vehicle_getSimple(jint, getImmunityFlags, GetVehicleImmunityFlags);
Java_Vehicle_doSimple(detonate, ExplodeVehicle);
Java_Vehicle_getSimple(jboolean, isWrecked, IsVehicleWrecked);

extern "C" JNIEXPORT void JNICALL Java_com_maxorator_vcmp_java_plugin_integration_vehicle_VehicleImpl_setPosition(JNIEnv* jni, jobject obj, jfloat X, jfloat Y, jfloat Z) {
	int vehicleId;

	if (!core->exc->ThreadCheck(jni));
	else if ((vehicleId = core->pools->vehicles->GetId(jni, obj)) == -1);
	else {
		api->SetVehiclePosition(vehicleId, X, Y, Z, 0);
	}
}

extern "C" JNIEXPORT jobject JNICALL Java_com_maxorator_vcmp_java_plugin_integration_vehicle_VehicleImpl_getPosition(JNIEnv* jni, jobject obj) {
	int vehicleId;

	if (!core->exc->ThreadCheck(jni));
	else if ((vehicleId = core->pools->vehicles->GetId(jni, obj)) == -1);
	else {
		float X = 0.0f, Y = 0.0f, Z = 0.0f;
		api->GetVehiclePosition(vehicleId, &X, &Y, &Z);
		return core->pools->CreateVector(jni, X, Y, Z);
	}

	return nullptr;
}

extern "C" JNIEXPORT void JNICALL Java_com_maxorator_vcmp_java_plugin_integration_vehicle_VehicleImpl_setRotation(JNIEnv* jni, jobject obj, jfloat X, jfloat Y, jfloat Z, jfloat W) {
	int vehicleId;

	if (!core->exc->ThreadCheck(jni));
	else if ((vehicleId = core->pools->vehicles->GetId(jni, obj)) == -1);
	else {
		api->SetVehicleRotation(vehicleId, X, Y, Z, W);
	}
}

extern "C" JNIEXPORT jobject JNICALL Java_com_maxorator_vcmp_java_plugin_integration_vehicle_VehicleImpl_getRotation(JNIEnv* jni, jobject obj) {
	int vehicleId;

	if (!core->exc->ThreadCheck(jni));
	else if ((vehicleId = core->pools->vehicles->GetId(jni, obj)) == -1);
	else {
		float X = 0.0f, Y = 0.0f, Z = 0.0f, W = 0.0f;
		api->GetVehicleRotation(vehicleId, &X, &Y, &Z, &W);
		return core->pools->CreateQuaternion(jni, X, Y, Z, W);
	}

	return nullptr;
}

extern "C" JNIEXPORT void JNICALL Java_com_maxorator_vcmp_java_plugin_integration_vehicle_VehicleImpl_setRotationEuler(JNIEnv* jni, jobject obj, jfloat X, jfloat Y, jfloat Z) {
	int vehicleId;

	if (!core->exc->ThreadCheck(jni));
	else if ((vehicleId = core->pools->vehicles->GetId(jni, obj)) == -1);
	else {
		api->SetVehicleRotationEuler(vehicleId, X, Y, Z);
	}
}

extern "C" JNIEXPORT jobject JNICALL Java_com_maxorator_vcmp_java_plugin_integration_vehicle_VehicleImpl_getRotationEuler(JNIEnv* jni, jobject obj) {
	int vehicleId;

	if (!core->exc->ThreadCheck(jni));
	else if ((vehicleId = core->pools->vehicles->GetId(jni, obj)) == -1);
	else {
		float X = 0.0f, Y = 0.0f, Z = 0.0f;
		api->GetVehicleRotationEuler(vehicleId, &X, &Y, &Z);
		return core->pools->CreateVector(jni, X, Y, Z);
	}

	return nullptr;
}

extern "C" JNIEXPORT void JNICALL Java_com_maxorator_vcmp_java_plugin_integration_vehicle_VehicleImpl_setSpeed(JNIEnv* jni, jobject obj, jfloat X, jfloat Y, jfloat Z, jboolean add, jboolean relative) {
	int vehicleId;

	if (!core->exc->ThreadCheck(jni));
	else if ((vehicleId = core->pools->vehicles->GetId(jni, obj)) == -1);
	else {
		api->SetVehicleSpeed(vehicleId, X, Y, Z, add, relative);
	}
}

extern "C" JNIEXPORT jobject JNICALL Java_com_maxorator_vcmp_java_plugin_integration_vehicle_VehicleImpl_getSpeed(JNIEnv* jni, jobject obj, jboolean relative) {
	int vehicleId;

	if (!core->exc->ThreadCheck(jni));
	else if ((vehicleId = core->pools->vehicles->GetId(jni, obj)) == -1);
	else {
		float X = 0.0f, Y = 0.0f, Z = 0.0f;
		api->GetVehicleSpeed(vehicleId, &X, &Y, &Z, relative);
		return core->pools->CreateVector(jni, X, Y, Z);
	}

	return nullptr;
}

extern "C" JNIEXPORT void JNICALL Java_com_maxorator_vcmp_java_plugin_integration_vehicle_VehicleImpl_setTurnSpeed(JNIEnv* jni, jobject obj, jfloat X, jfloat Y, jfloat Z, jboolean add, jboolean relative) {
	int vehicleId;

	if (!core->exc->ThreadCheck(jni));
	else if ((vehicleId = core->pools->vehicles->GetId(jni, obj)) == -1);
	else {
		api->SetVehicleTurnSpeed(vehicleId, X, Y, Z, add, relative);
	}
}

extern "C" JNIEXPORT jobject JNICALL Java_com_maxorator_vcmp_java_plugin_integration_vehicle_VehicleImpl_getTurnSpeed(JNIEnv* jni, jobject obj, jboolean relative) {
	int vehicleId;

	if (!core->exc->ThreadCheck(jni));
	else if ((vehicleId = core->pools->vehicles->GetId(jni, obj)) == -1);
	else {
		float X = 0.0f, Y = 0.0f, Z = 0.0f;
		api->GetVehicleTurnSpeed(vehicleId, &X, &Y, &Z, relative);
		return core->pools->CreateVector(jni, X, Y, Z);
	}

	return nullptr;
}

extern "C" JNIEXPORT void JNICALL Java_com_maxorator_vcmp_java_plugin_integration_vehicle_VehicleImpl_setSpawnPosition(JNIEnv* jni, jobject obj, jfloat X, jfloat Y, jfloat Z) {
	int vehicleId;

	if (!core->exc->ThreadCheck(jni));
	else if ((vehicleId = core->pools->vehicles->GetId(jni, obj)) == -1);
	else {
		api->SetVehicleSpawnPosition(vehicleId, X, Y, Z);
	}
}

extern "C" JNIEXPORT jobject JNICALL Java_com_maxorator_vcmp_java_plugin_integration_vehicle_VehicleImpl_getSpawnPosition(JNIEnv* jni, jobject obj) {
	int vehicleId;

	if (!core->exc->ThreadCheck(jni));
	else if ((vehicleId = core->pools->vehicles->GetId(jni, obj)) == -1);
	else {
		float X = 0.0f, Y = 0.0f, Z = 0.0f;
		api->GetVehicleSpawnPosition(vehicleId, &X, &Y, &Z);
		return core->pools->CreateVector(jni, X, Y, Z);
	}

	return nullptr;
}

extern "C" JNIEXPORT void JNICALL Java_com_maxorator_vcmp_java_plugin_integration_vehicle_VehicleImpl_setSpawnRotation(JNIEnv* jni, jobject obj, jfloat X, jfloat Y, jfloat Z, jfloat W) {
	int vehicleId;

	if (!core->exc->ThreadCheck(jni));
	else if ((vehicleId = core->pools->vehicles->GetId(jni, obj)) == -1);
	else {
		api->SetVehicleSpawnRotation(vehicleId, X, Y, Z, W);
	}
}

extern "C" JNIEXPORT jobject JNICALL Java_com_maxorator_vcmp_java_plugin_integration_vehicle_VehicleImpl_getSpawnRotation(JNIEnv* jni, jobject obj) {
	int vehicleId;

	if (!core->exc->ThreadCheck(jni));
	else if ((vehicleId = core->pools->vehicles->GetId(jni, obj)) == -1);
	else {
		float X = 0.0f, Y = 0.0f, Z = 0.0f, W = 0.0f;
		api->GetVehicleSpawnRotation(vehicleId, &X, &Y, &Z, &W);
		return core->pools->CreateQuaternion(jni, X, Y, Z, W);
	}

	return nullptr;
}

extern "C" JNIEXPORT void JNICALL Java_com_maxorator_vcmp_java_plugin_integration_vehicle_VehicleImpl_setSpawnRotationEuler(JNIEnv* jni, jobject obj, jfloat X, jfloat Y, jfloat Z) {
	int vehicleId;

	if (!core->exc->ThreadCheck(jni));
	else if ((vehicleId = core->pools->vehicles->GetId(jni, obj)) == -1);
	else {
		api->SetVehicleSpawnRotationEuler(vehicleId, X, Y, Z);
	}
}

extern "C" JNIEXPORT jobject JNICALL Java_com_maxorator_vcmp_java_plugin_integration_vehicle_VehicleImpl_getSpawnRotationEuler(JNIEnv* jni, jobject obj) {
	int vehicleId;

	if (!core->exc->ThreadCheck(jni));
	else if ((vehicleId = core->pools->vehicles->GetId(jni, obj)) == -1);
	else {
		float X = 0.0f, Y = 0.0f, Z = 0.0f;
		api->GetVehicleSpawnRotationEuler(vehicleId, &X, &Y, &Z);
		return core->pools->CreateVector(jni, X, Y, Z);
	}

	return nullptr;
}

Java_Vehicle_setSimple(jint, setIdleRespawnTimeout, SetVehicleIdleRespawnTimer);
Java_Vehicle_getSimple(jint, getIdleRespawnTimeout, GetVehicleIdleRespawnTimer);
Java_Vehicle_setSimple(jfloat, setHealth, SetVehicleHealth);
Java_Vehicle_getSimple(jfloat, getHealth, GetVehicleHealth);

extern "C" JNIEXPORT void JNICALL Java_com_maxorator_vcmp_java_plugin_integration_vehicle_VehicleImpl_setColours(JNIEnv* jni, jobject obj, jint primary, jint secondary) {
	int vehicleId;

	if (!core->exc->ThreadCheck(jni));
	else if ((vehicleId = core->pools->vehicles->GetId(jni, obj)) == -1);
	else {
		api->SetVehicleColour(vehicleId, primary, secondary);
	}
}

extern "C" JNIEXPORT jobject JNICALL Java_com_maxorator_vcmp_java_plugin_integration_vehicle_VehicleImpl_getColours(JNIEnv* jni, jobject obj) {
	int vehicleId;

	if (!core->exc->ThreadCheck(jni));
	else if ((vehicleId = core->pools->vehicles->GetId(jni, obj)) == -1);
	else {
		int primary = 0, secondary = 0;
		api->GetVehicleColour(vehicleId, &primary, &secondary);
		return core->pools->CreateVehicleColours(jni, primary, secondary);
	}

	return nullptr;
}

typedef int (*OPTS_ToggleVehicleOption) (int vehicleId, unsigned int value);

extern "C" JNIEXPORT void JNICALL Java_com_maxorator_vcmp_java_plugin_integration_vehicle_VehicleImpl_setOption(JNIEnv* jni, jobject obj, jint option, jboolean value) {
	int vehicleId;

	if (!core->exc->ThreadCheck(jni));
	else if ((vehicleId = core->pools->vehicles->GetId(jni, obj)) == -1);
	else {
		api->SetVehicleOption(vehicleId, (vcmpVehicleOption)option, value);
	}
	return;
}

typedef unsigned int (*OPTS_GetVehicleOption) (int playerId);

extern "C" JNIEXPORT jboolean JNICALL Java_com_maxorator_vcmp_java_plugin_integration_vehicle_VehicleImpl_getOption(JNIEnv* jni, jobject obj, jint option) {
	int vehicleId;

	if (!core->exc->ThreadCheck(jni));
	else if ((vehicleId = core->pools->vehicles->GetId(jni, obj)) == -1);
	else {
		return !!api->GetVehicleOption(vehicleId, (vcmpVehicleOption)option);
	}
	return false;
}

Java_Vehicle_setSimple(jint, setDamage, SetVehicleDamageData);
Java_Vehicle_getSimple(jint, getDamageHex, GetVehicleDamageData);
Java_Vehicle_setSimple(jint, setRadio, SetVehicleRadio);
Java_Vehicle_getSimple(jint, getRadio, GetVehicleRadio);
Java_Vehicle_getSimple(jint, getLightsData, GetVehicleLightsData);
Java_Vehicle_setSimple(jint, setLightsData, SetVehicleLightsData);

extern "C" JNIEXPORT jobject JNICALL Java_com_maxorator_vcmp_java_plugin_integration_vehicle_VehicleImpl_getTurretRotation(JNIEnv* jni, jobject obj) {
	int vehicleId;

	if (!core->exc->ThreadCheck(jni));
	else if ((vehicleId = core->pools->vehicles->GetId(jni, obj)) == -1);
	else {
		float horizontal, vertical;
		api->GetVehicleTurretRotation(vehicleId, &horizontal, &vertical);
		return core->pools->CreateRotation2d(jni, horizontal, vertical);
	}

	return nullptr;
}

extern "C" JNIEXPORT jboolean JNICALL Java_com_maxorator_vcmp_java_plugin_integration_vehicle_VehicleImpl_handlingRuleExists(JNIEnv* jni, jobject obj, jint index) {
	int vehicleId;

	if (!core->exc->ThreadCheck(jni));
	else if ((vehicleId = core->pools->vehicles->GetId(jni, obj)) == -1);
	else {
		return api->ExistsInstHandlingRule(vehicleId, index);
	}

	return false;
}

extern "C" JNIEXPORT void JNICALL Java_com_maxorator_vcmp_java_plugin_integration_vehicle_VehicleImpl_setHandlingRule(JNIEnv* jni, jobject obj, jint index, jdouble value) {
	int vehicleId;

	if (!core->exc->ThreadCheck(jni));
	else if ((vehicleId = core->pools->vehicles->GetId(jni, obj)) == -1);
	else {
		api->SetInstHandlingRule(vehicleId, index, value);
	}
}

extern "C" JNIEXPORT jdouble JNICALL Java_com_maxorator_vcmp_java_plugin_integration_vehicle_VehicleImpl_getHandlingRule(JNIEnv* jni, jobject obj, jint index) {
	int vehicleId;

	if (!core->exc->ThreadCheck(jni));
	else if ((vehicleId = core->pools->vehicles->GetId(jni, obj)) == -1);
	else {
		return api->GetInstHandlingRule(vehicleId, index);
	}

	return 0;
}

Java_Vehicle_setSimple(jint, resetHandlingRule, ResetInstHandlingRule);
Java_Vehicle_doSimple(resetHandling, ResetInstHandling);
