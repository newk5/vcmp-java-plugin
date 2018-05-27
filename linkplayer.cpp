#include "main.h"
#include "logging.h"
#include "javastring.h"

#define Java_Player_getSimple(type, methodName, apiName) \
	extern "C" JNIEXPORT type JNICALL Java_com_maxorator_vcmp_java_plugin_integration_player_PlayerImpl_##methodName(JNIEnv* jni, jobject obj) { \
		int playerId; \
		if (!core->exc->ThreadCheck(jni)); \
		else if ((playerId = core->pools->players->GetId(jni, obj)) == -1); \
		else { \
			return api->apiName(playerId); \
		} \
		return 0; \
	}

#define Java_Player_setSimple(type, methodName, apiName) \
	extern "C" JNIEXPORT void JNICALL Java_com_maxorator_vcmp_java_plugin_integration_player_PlayerImpl_##methodName(JNIEnv* jni, jobject obj, type value) { \
		int playerId; \
		if (!core->exc->ThreadCheck(jni)); \
		else if ((playerId = core->pools->players->GetId(jni, obj)) == -1); \
		else { \
			api->apiName(playerId, value); \
		} \
	}

#define Java_Player_doSimple(methodName, apiName) \
	extern "C" JNIEXPORT void JNICALL Java_com_maxorator_vcmp_java_plugin_integration_player_PlayerImpl_##methodName(JNIEnv* jni, jobject obj) { \
		int playerId; \
		if (!core->exc->ThreadCheck(jni)); \
		else if ((playerId = core->pools->players->GetId(jni, obj)) == -1); \
		else { \
			api->apiName(playerId); \
		} \
	}

extern "C" JNIEXPORT jobject JNICALL Java_com_maxorator_vcmp_java_plugin_integration_player_PlayerImpl_get(JNIEnv* jni, jclass klass, jint playerId) {
	if (!core->exc->ThreadCheck(jni)) return nullptr;

	if (!api->IsPlayerConnected(playerId)) {
		return nullptr;
	}

	return core->pools->players->Get(jni, playerId);
}

Java_Player_getSimple(jboolean, isAdmin, IsPlayerAdmin);
Java_Player_setSimple(jboolean, setAdmin, SetPlayerAdmin);

extern "C" JNIEXPORT jstring JNICALL Java_com_maxorator_vcmp_java_plugin_integration_player_PlayerImpl_getIP(JNIEnv* jni, jobject obj) {
	int playerId;

	if (!core->exc->ThreadCheck(jni));
	else if ((playerId = core->pools->players->GetId(jni, obj)) == -1);
	else {
		char ipAddress[64];
		if (api->GetPlayerIP(playerId, ipAddress, sizeof(ipAddress)) != vcmpErrorNone) {
			return nullptr;
		}

		return core->env->NewStringUTF(ipAddress);
	}

	return nullptr;
}

Java_Player_doSimple(kick, KickPlayer);
Java_Player_doSimple(ban, BanPlayer);

Java_Player_getSimple(jboolean, isSpawned, IsPlayerSpawned);

extern "C" JNIEXPORT jboolean JNICALL Java_com_maxorator_vcmp_java_plugin_integration_player_PlayerImpl_isStreamedForPlayer(JNIEnv* jni, jobject obj, jobject otherPlayer) {
	int playerId, otherPlayerId;

	if (!core->exc->ThreadCheck(jni));
	else if ((playerId = core->pools->players->GetId(jni, obj)) == -1);
	else if ((otherPlayerId = core->pools->vehicles->GetId(jni, otherPlayer)) == -1);
	else {
		return api->IsPlayerStreamedForPlayer(playerId, otherPlayerId);
	}
	return false;
}

Java_Player_getSimple(jlong, getUniqueId, GetPlayerKey);
Java_Player_setSimple(jint, setWorld, SetPlayerWorld);
Java_Player_getSimple(jint, getWorld, GetPlayerWorld);
Java_Player_setSimple(jint, setSecondaryWorld, SetPlayerSecondaryWorld);
Java_Player_getSimple(jint, getSecondaryWorld, GetPlayerSecondaryWorld);
Java_Player_getSimple(jint, getUniqueWorld, GetPlayerUniqueWorld);

extern "C" JNIEXPORT jboolean JNICALL Java_com_maxorator_vcmp_java_plugin_integration_player_PlayerImpl_isCompatibleWithWorld(JNIEnv* jni, jobject obj, jint worldId) {
	int playerId;

	if (!core->exc->ThreadCheck(jni));
	else if ((playerId = core->pools->players->GetId(jni, obj)) == -1);
	else {
		return api->IsPlayerWorldCompatible(playerId, worldId);
	}

	return false;
}

Java_Player_getSimple(jint, getState, GetPlayerState);

extern "C" JNIEXPORT jstring JNICALL Java_com_maxorator_vcmp_java_plugin_integration_player_PlayerImpl_getName(JNIEnv* jni, jobject obj) {
	int playerId;

	if (!core->exc->ThreadCheck(jni));
	else if ((playerId = core->pools->players->GetId(jni, obj)) == -1);
	else {
		char playerNameBuffer[64];
		if (api->GetPlayerName(playerId, playerNameBuffer, sizeof(playerNameBuffer)) != vcmpErrorNone) {
			return nullptr;
		}

		return core->env->NewStringUTF(playerNameBuffer);
	}

	return nullptr;
}

extern "C" JNIEXPORT jboolean JNICALL Java_com_maxorator_vcmp_java_plugin_integration_player_PlayerImpl_setName(JNIEnv* jni, jobject obj, jstring nameString) {
	int playerId;

	if (!core->exc->ThreadCheck(jni));
	else if ((playerId = core->pools->players->GetId(jni, obj)) == -1);
	else {
		return api->SetPlayerName(playerId, JavaString(jni, nameString).text);
	}

	return false;
}

Java_Player_setSimple(jint, setTeam, SetPlayerTeam);
Java_Player_getSimple(jint, getTeam, GetPlayerTeam);
Java_Player_setSimple(jint, setSkin, SetPlayerSkin);
Java_Player_getSimple(jint, getSkin, GetPlayerSkin);
Java_Player_setSimple(jint, setColour, SetPlayerColour);
Java_Player_getSimple(jint, getColourHex, GetPlayerColour);
Java_Player_doSimple(forceSpawn, ForcePlayerSpawn);
Java_Player_doSimple(forceSelect, ForcePlayerSelect);
Java_Player_setSimple(jint, giveMoney, GivePlayerMoney);
Java_Player_setSimple(jint, setMoney, SetPlayerMoney);
Java_Player_getSimple(jint, getMoney, GetPlayerMoney);
Java_Player_setSimple(jint, setScore, SetPlayerScore);
Java_Player_getSimple(jint, getScore, GetPlayerScore);
Java_Player_getSimple(jint, getPing, GetPlayerPing);
Java_Player_getSimple(jboolean, isTyping, IsPlayerTyping);
Java_Player_getSimple(jdouble, getFPS, GetPlayerFPS);

extern "C" JNIEXPORT jstring JNICALL Java_com_maxorator_vcmp_java_plugin_integration_player_PlayerImpl_getUID(JNIEnv* jni, jobject obj) {
	int playerId;

	if (!core->exc->ThreadCheck(jni));
	else if ((playerId = core->pools->players->GetId(jni, obj)) == -1);
	else {
		char uidBuffer[64];
		if (api->GetPlayerUID(playerId, uidBuffer, sizeof(uidBuffer)) != vcmpErrorNone) {
			return nullptr;
		}

		return core->env->NewStringUTF(uidBuffer);
	}

	return nullptr;
}

extern "C" JNIEXPORT jstring JNICALL Java_com_maxorator_vcmp_java_plugin_integration_player_PlayerImpl_getUID2(JNIEnv* jni, jobject obj) {
	int playerId;

	if (!core->exc->ThreadCheck(jni));
	else if ((playerId = core->pools->players->GetId(jni, obj)) == -1);
	else {
		char uidBuffer[64];
		if (api->GetPlayerUID2(playerId, uidBuffer, sizeof(uidBuffer)) != vcmpErrorNone) {
			return nullptr;
		}

		return core->env->NewStringUTF(uidBuffer);
	}

	return nullptr;
}

Java_Player_setSimple(jfloat, setHealth, SetPlayerHealth);
Java_Player_getSimple(jfloat, getHealth, GetPlayerHealth);
Java_Player_setSimple(jfloat, setArmour, SetPlayerArmour);
Java_Player_getSimple(jfloat, getArmour, GetPlayerArmour);
Java_Player_setSimple(jint, setImmunityFlags, SetPlayerImmunityFlags);
Java_Player_getSimple(jint, getImmunityFlags, GetPlayerImmunityFlags);

extern "C" JNIEXPORT jobject JNICALL Java_com_maxorator_vcmp_java_plugin_integration_player_PlayerImpl_getPosition(JNIEnv* jni, jobject obj) {
	int playerId;

	if (!core->exc->ThreadCheck(jni)) return nullptr;
	else if ((playerId = core->pools->players->GetId(jni, obj)) == -1) return nullptr;

	float X = 0.0f, Y = 0.0f, Z = 0.0f;
	api->GetPlayerPosition(playerId, &X, &Y, &Z);

	return core->pools->CreateVector(jni, X, Y, Z);
}

extern "C" JNIEXPORT void JNICALL Java_com_maxorator_vcmp_java_plugin_integration_player_PlayerImpl_setPosition(JNIEnv* jni, jobject obj, jfloat X, jfloat Y, jfloat Z) {
	int playerId;

	if (!core->exc->ThreadCheck(jni)) return;
	else if ((playerId = core->pools->players->GetId(jni, obj)) == -1) return;

	api->SetPlayerPosition(playerId, X, Y, Z);
}

extern "C" JNIEXPORT jobject JNICALL Java_com_maxorator_vcmp_java_plugin_integration_player_PlayerImpl_getSpeed(JNIEnv* jni, jobject obj) {
	int playerId;

	if (!core->exc->ThreadCheck(jni)) return nullptr;
	else if ((playerId = core->pools->players->GetId(jni, obj)) == -1) return nullptr;

	float X = 0.0f, Y = 0.0f, Z = 0.0f;
	api->GetPlayerSpeed(playerId, &X, &Y, &Z);

	return core->pools->CreateVector(jni, X, Y, Z);
}

extern "C" JNIEXPORT void JNICALL Java_com_maxorator_vcmp_java_plugin_integration_player_PlayerImpl_setSpeed(JNIEnv* jni, jobject obj, jfloat X, jfloat Y, jfloat Z) {
	int playerId;

	if (!core->exc->ThreadCheck(jni)) return;
	else if ((playerId = core->pools->players->GetId(jni, obj)) == -1) return;

	api->SetPlayerSpeed(playerId, X, Y, Z);
}

extern "C" JNIEXPORT void JNICALL Java_com_maxorator_vcmp_java_plugin_integration_player_PlayerImpl_addSpeed(JNIEnv* jni, jobject obj, jfloat X, jfloat Y, jfloat Z) {
	int playerId;

	if (!core->exc->ThreadCheck(jni)) return;
	else if ((playerId = core->pools->players->GetId(jni, obj)) == -1) return;

	api->AddPlayerSpeed(playerId, X, Y, Z);
}

Java_Player_setSimple(jfloat, setHeading, SetPlayerHeading);
Java_Player_getSimple(jfloat, getHeading, GetPlayerHeading);

extern "C" JNIEXPORT void JNICALL Java_com_maxorator_vcmp_java_plugin_integration_player_PlayerImpl_setAlpha(JNIEnv* jni, jobject obj, jint value, jint fadeTime) {
	int playerId;

	if (!core->exc->ThreadCheck(jni));
	else if ((playerId = core->pools->players->GetId(jni, obj)) == -1);
	else {
		api->SetPlayerAlpha(playerId, value, fadeTime);
	}
}

Java_Player_getSimple(jint, getAlpha, GetPlayerAlpha);
Java_Player_getSimple(jboolean, isOnFire, IsPlayerOnFire);
Java_Player_getSimple(jboolean, isCrouching, IsPlayerCrouching);
Java_Player_getSimple(jint, getAction, GetPlayerAction);
Java_Player_getSimple(jint, getGameKeys, GetPlayerGameKeys);

extern "C" JNIEXPORT jobject JNICALL Java_com_maxorator_vcmp_java_plugin_integration_player_PlayerImpl_getAimPosition(JNIEnv* jni, jobject obj) {
	int playerId;

	if (!core->exc->ThreadCheck(jni)) return nullptr;
	else if ((playerId = core->pools->players->GetId(jni, obj)) == -1) return nullptr;

	float X = 0.0f, Y = 0.0f, Z = 0.0f;
	api->GetPlayerAimPosition(playerId, &X, &Y, &Z);

	return core->pools->CreateVector(jni, X, Y, Z);
}

extern "C" JNIEXPORT jobject JNICALL Java_com_maxorator_vcmp_java_plugin_integration_player_PlayerImpl_getAimDirection(JNIEnv* jni, jobject obj) {
	int playerId;

	if (!core->exc->ThreadCheck(jni)) return nullptr;
	else if ((playerId = core->pools->players->GetId(jni, obj)) == -1) return nullptr;

	float X = 0.0f, Y = 0.0f, Z = 0.0f;
	api->GetPlayerAimDirection(playerId, &X, &Y, &Z);

	return core->pools->CreateVector(jni, X, Y, Z);
}

extern "C" JNIEXPORT jboolean JNICALL Java_com_maxorator_vcmp_java_plugin_integration_player_PlayerImpl_putInVehicle(JNIEnv* jni, jobject obj, jobject vehicle, jint slot, jboolean makeRoom, jboolean warp) {
	int playerId, vehicleId;

	if (!core->exc->ThreadCheck(jni));
	else if ((playerId = core->pools->players->GetId(jni, obj)) == -1);
	else if ((vehicleId = core->pools->vehicles->GetId(jni, vehicle)) == -1);
	else {
		return api->PutPlayerInVehicle(playerId, vehicleId, slot, makeRoom, warp);
	}
	return false;
}

Java_Player_doSimple(removeFromVehicle, RemovePlayerFromVehicle);
Java_Player_getSimple(jint, getInVehicleStatus, GetPlayerInVehicleStatus);
Java_Player_getSimple(jint, getInVehicleSlot, GetPlayerInVehicleSlot);

extern "C" JNIEXPORT jobject JNICALL Java_com_maxorator_vcmp_java_plugin_integration_player_PlayerImpl_getVehicle(JNIEnv* jni, jobject obj) {
	int playerId;

	if (!core->exc->ThreadCheck(jni));
	else if ((playerId = core->pools->players->GetId(jni, obj)) == -1);
	else {
		int vehicleId = api->GetPlayerVehicleId(playerId);
		return vehicleId == 0 ? nullptr : core->pools->vehicles->Get(jni, vehicleId);
	}
	return nullptr;
}

typedef int (*OPTS_TogglePlayerOption) (int playerId, unsigned int value);

extern "C" JNIEXPORT void JNICALL Java_com_maxorator_vcmp_java_plugin_integration_player_PlayerImpl_setOption(JNIEnv* jni, jobject obj, jint option, jboolean value) {
	int playerId;

	if (!core->exc->ThreadCheck(jni));
	else if ((playerId = core->pools->players->GetId(jni, obj)) == -1);
	else {
		api->SetPlayerOption(playerId, (vcmpPlayerOption)option, value ? 1 : 0);
	}
	return;
}

typedef unsigned int (*OPTS_GetPlayerOption) (int playerId);

extern "C" JNIEXPORT jboolean JNICALL Java_com_maxorator_vcmp_java_plugin_integration_player_PlayerImpl_getOption(JNIEnv* jni, jobject obj, jint option) {
	int playerId;

	if (!core->exc->ThreadCheck(jni));
	else if ((playerId = core->pools->players->GetId(jni, obj)) == -1);
	else {
		return api->GetPlayerOption(playerId, (vcmpPlayerOption)option);
	}
	return false;
}

extern "C" JNIEXPORT void JNICALL Java_com_maxorator_vcmp_java_plugin_integration_player_PlayerImpl_giveWeapon(JNIEnv* jni, jobject obj, jint weapon, jint ammo) {
	int playerId;

	if (!core->exc->ThreadCheck(jni));
	else if ((playerId = core->pools->players->GetId(jni, obj)) == -1);
	else {
		api->GivePlayerWeapon(playerId, weapon, ammo);
	}
}

extern "C" JNIEXPORT void JNICALL Java_com_maxorator_vcmp_java_plugin_integration_player_PlayerImpl_setWeapon(JNIEnv* jni, jobject obj, jint weapon, jint ammo) {
	int playerId;

	if (!core->exc->ThreadCheck(jni));
	else if ((playerId = core->pools->players->GetId(jni, obj)) == -1);
	else {
		api->SetPlayerWeapon(playerId, weapon, ammo);
	}
}

Java_Player_getSimple(jint, getWeapon, GetPlayerWeapon);
Java_Player_getSimple(jint, getWeaponAmmo, GetPlayerWeaponAmmo);
Java_Player_setSimple(jint, setWeaponSlot, SetPlayerWeaponSlot);
Java_Player_getSimple(jint, getWeaponSlot, GetPlayerWeaponSlot);

extern "C" JNIEXPORT jint JNICALL Java_com_maxorator_vcmp_java_plugin_integration_player_PlayerImpl_getWeaponAtSlot(JNIEnv* jni, jobject obj, jint slot) {
	int playerId;

	if (!core->exc->ThreadCheck(jni));
	else if ((playerId = core->pools->players->GetId(jni, obj)) == -1);
	else {
		api->GetPlayerWeaponAtSlot(playerId, slot);
	}
	return 0;
}

extern "C" JNIEXPORT jint JNICALL Java_com_maxorator_vcmp_java_plugin_integration_player_PlayerImpl_getAmmoAtSlot(JNIEnv* jni, jobject obj, jint slot) {
	int playerId;

	if (!core->exc->ThreadCheck(jni));
	else if ((playerId = core->pools->players->GetId(jni, obj)) == -1);
	else {
		api->GetPlayerAmmoAtSlot(playerId, slot);
	}
	return 0;
}

Java_Player_setSimple(jint, removeWeapon, RemovePlayerWeapon);
Java_Player_doSimple(removeAllWeapons, RemoveAllWeapons);

extern "C" JNIEXPORT void JNICALL Java_com_maxorator_vcmp_java_plugin_integration_player_PlayerImpl_setCameraPosition(JNIEnv* jni, jobject obj, jfloat posX, jfloat posY, jfloat posZ, jfloat lookX, jfloat lookY, jfloat lookZ) {
	int playerId;

	if (!core->exc->ThreadCheck(jni));
	else if ((playerId = core->pools->players->GetId(jni, obj)) == -1);
	else {
		api->SetCameraPosition(playerId, posX, posY, posZ, lookX, lookY, lookZ);
	}
}

Java_Player_doSimple(restoreCamera, RestoreCamera);
Java_Player_getSimple(jboolean, isCameraLocked, IsCameraLocked);

extern "C" JNIEXPORT void JNICALL Java_com_maxorator_vcmp_java_plugin_integration_player_PlayerImpl_setAnimation(JNIEnv* jni, jobject obj, jint groupId, jint animationId) {
	int playerId;

	if (!core->exc->ThreadCheck(jni));
	else if ((playerId = core->pools->players->GetId(jni, obj)) == -1);
	else {
		api->SetPlayerAnimation(playerId, groupId, animationId);
	}
}

Java_Player_setSimple(jint, setWantedLevel, SetPlayerWantedLevel);
Java_Player_getSimple(jint, getWantedLevel, GetPlayerWantedLevel);

extern "C" JNIEXPORT jobject JNICALL Java_com_maxorator_vcmp_java_plugin_integration_player_PlayerImpl_getStandingOnVehicle(JNIEnv* jni, jobject obj) {
	int playerId;

	if (!core->exc->ThreadCheck(jni));
	else if ((playerId = core->pools->players->GetId(jni, obj)) == -1);
	else {
		return core->pools->vehicles->Get(jni, api->GetPlayerStandingOnVehicle(playerId));
	}
	return nullptr;
}

extern "C" JNIEXPORT jobject JNICALL Java_com_maxorator_vcmp_java_plugin_integration_player_PlayerImpl_getStandingOnObject(JNIEnv* jni, jobject obj) {
	int playerId;

	if (!core->exc->ThreadCheck(jni));
	else if ((playerId = core->pools->players->GetId(jni, obj)) == -1);
	else {
		return core->pools->objects->Get(jni, api->GetPlayerStandingOnObject(playerId));
	}
	return nullptr;
}

Java_Player_getSimple(jboolean, isAway, IsPlayerAway);

extern "C" JNIEXPORT jobject JNICALL Java_com_maxorator_vcmp_java_plugin_integration_player_PlayerImpl_getSpectateTarget(JNIEnv* jni, jobject obj) {
	int playerId;

	if (!core->exc->ThreadCheck(jni));
	else if ((playerId = core->pools->players->GetId(jni, obj)) == -1);
	else {
		return core->pools->players->Get(jni, api->GetPlayerSpectateTarget(playerId));
	}
	return nullptr;
}

extern "C" JNIEXPORT void JNICALL Java_com_maxorator_vcmp_java_plugin_integration_player_PlayerImpl_setSpectateTarget(JNIEnv* jni, jobject obj, jobject otherPlayer) {
	int playerId;

	if (!core->exc->ThreadCheck(jni));
	else if ((playerId = core->pools->players->GetId(jni, obj)) == -1);
	else {
		int otherPlayerId = core->pools->players->GetId(jni, otherPlayer);
		api->SetPlayerSpectateTarget(playerId, otherPlayerId);
	}
}

Java_Player_getSimple(jint, getPlayerClass, GetPlayerClass);

extern "C" JNIEXPORT jboolean JNICALL Java_com_maxorator_vcmp_java_plugin_integration_player_PlayerImpl_redirectToServer(JNIEnv* jni, jobject obj, jstring address, int port, jstring nick, jstring serverPassword, jstring userPassword) {
	int playerId;

	if (!core->exc->ThreadCheck(jni));
	else if ((playerId = core->pools->players->GetId(jni, obj)) == -1);
	else {
		return api->RedirectPlayerToServer(playerId, JavaString(jni, address).text, port, JavaString(jni, nick).text, JavaString(jni, serverPassword).text, JavaString(jni, userPassword).text);
	}

	return false;
}
