#include "main.h"
#include "javaeventproxy.h"
#include "javastring.h"
#include "logging.h"
#include "common.h"

#define Java_Server_getSimple(type, methodName, apiName) \
	extern "C" JNIEXPORT type JNICALL Java_com_maxorator_vcmp_java_plugin_integration_server_ServerImpl_##methodName(JNIEnv* jni, jobject obj) { \
		if (!core->exc->ThreadCheck(jni)); \
		else { \
			return api->apiName(); \
		} \
		return 0; \
	}

#define Java_Server_setSimple(type, methodName, apiName) \
	extern "C" JNIEXPORT void JNICALL Java_com_maxorator_vcmp_java_plugin_integration_server_ServerImpl_##methodName(JNIEnv* jni, jobject obj, type value) { \
		if (!core->exc->ThreadCheck(jni)); \
		else { \
			api->apiName(value); \
		} \
	}

#define Java_Server_doSimple(methodName, apiName) \
	extern "C" JNIEXPORT void JNICALL Java_com_maxorator_vcmp_java_plugin_integration_server_ServerImpl_##methodName(JNIEnv* jni, jobject obj) { \
		if (!core->exc->ThreadCheck(jni)); \
		else { \
			api->apiName(); \
		} \
	}

extern "C" JNIEXPORT void JNICALL Java_com_maxorator_vcmp_java_plugin_integration_server_ServerImpl_reloadScript(JNIEnv* jni, jobject obj) {
	if (!core->exc->ThreadCheck(jni)) return;

	JavaEventProxy::FlagForRestart();
}

extern "C" JNIEXPORT void JNICALL Java_com_maxorator_vcmp_java_plugin_integration_server_ServerImpl_startThreadSynced(JNIEnv* jni, jobject obj) {
	core->exc->StartThreadSynced();
}

extern "C" JNIEXPORT void JNICALL Java_com_maxorator_vcmp_java_plugin_integration_server_ServerImpl_endThreadSynced(JNIEnv* jni, jobject obj) {
	if (!core->exc->ThreadCheck(jni)) return;

	core->exc->EndThreadSynced();
}

extern "C" JNIEXPORT jboolean JNICALL Java_com_maxorator_vcmp_java_plugin_integration_server_ServerImpl_rewireEvents(JNIEnv* jni, jobject obj, jobject handler, jlong flags) {
	if (!core->exc->ThreadCheckStrict(jni)) return JNI_FALSE;

	if (handler == nullptr) {
		Logging::Log(LogError, __FUNCTION__, "null EventHandler is not valid.");
		return JNI_FALSE;
	}

	JavaEventHandler* newEventHandler = new JavaEventHandler();
	if (!newEventHandler->BindToClasses(false, flags, handler)) {
		Logging::Log(LogError, __FUNCTION__, "Setting up new event handler failed, aborting rewire.");
		return JNI_FALSE;
	}

	JavaEventHandler* oldHandler = core->eventHandler;
	core->eventHandler = newEventHandler;
	delete oldHandler;

	return JNI_TRUE;
}

extern "C" JNIEXPORT void JNICALL Java_com_maxorator_vcmp_java_plugin_integration_server_ServerImpl_sendPluginCommand(JNIEnv* jni, jobject obj, jint type, jstring message) {
	if (!core->exc->ThreadCheck(jni)) return;
	else if (!core->exc->NullCheck(jni, message)) return;

	api->SendPluginCommand(type, "%s", JavaString(jni, message).text);
}

extern "C" JNIEXPORT jboolean JNICALL Java_com_maxorator_vcmp_java_plugin_integration_server_ServerImpl_sendScriptData(JNIEnv* jni, jobject obj, jobject player, jbyteArray bytes) {
	int playerId = -1;

	if (!core->exc->ThreadCheck(jni));
	else if (!core->exc->NullCheck(jni, bytes));
	else if (player != nullptr && (playerId = core->pools->players->GetId(jni, player)) == -1);
	else {
		jsize length = jni->GetArrayLength(bytes);
		jbyte* byteBuffer = jni->GetByteArrayElements(bytes, nullptr);
		jboolean result = api->SendClientScriptData(playerId, byteBuffer, length) == vcmpErrorNone;
		jni->ReleaseByteArrayElements(bytes, byteBuffer, 0);
		return result;
	}

	return false;
}


extern "C" JNIEXPORT void JNICALL Java_com_maxorator_vcmp_java_plugin_integration_server_ServerImpl_sendClientMessage(JNIEnv* jni, jobject obj, jobject player, jint colour, jstring message) {
	int playerId = -1;

	if (!core->exc->ThreadCheck(jni)) return;
	else if (!core->exc->NullCheck(jni, message)) return;
	else if (player != nullptr && (playerId = core->pools->players->GetId(jni, player)) == -1) return;

	api->SendClientMessage(playerId, Common::ConvertArgbToRgba((uint32_t)colour), "%s", JavaString(jni, message).text);
}

extern "C" JNIEXPORT void JNICALL Java_com_maxorator_vcmp_java_plugin_integration_server_ServerImpl_sendGameMessage(JNIEnv* jni, jobject obj, jobject player, jint type, jstring message) {
	int playerId = -1;

	if (!core->exc->ThreadCheck(jni)) return;
	else if (!core->exc->NullCheck(jni, message)) return;
	else if (player != nullptr && (playerId = core->pools->players->GetId(jni, player)) == -1) return;

	api->SendGameMessage(playerId, type, "%s", JavaString(jni, message).text);
}

Java_Server_getSimple(jint, getServerVersion, GetServerVersion);

extern "C" JNIEXPORT jint JNICALL Java_com_maxorator_vcmp_java_plugin_integration_server_ServerImpl_getServerPort(JNIEnv* jni, jobject obj) {
	if (!core->exc->ThreadCheck(jni)) return 0;

	ServerSettings settings;
	api->GetServerSettings(&settings);

	return settings.port;
}

extern "C" JNIEXPORT void JNICALL Java_com_maxorator_vcmp_java_plugin_integration_server_ServerImpl_setServerName(JNIEnv* jni, jobject obj, jstring serverName) {
	if (!core->exc->ThreadCheck(jni)) return;
	else if (!core->exc->NullCheck(jni, serverName)) return;

	api->SetServerName(JavaString(jni, serverName).text);
}

extern "C" JNIEXPORT jstring JNICALL Java_com_maxorator_vcmp_java_plugin_integration_server_ServerImpl_getServerName(JNIEnv* jni, jobject obj) {
	if (!core->exc->ThreadCheck(jni)) return nullptr;

	char serverNameBuffer[256];
	serverNameBuffer[0] = '\0';

	api->GetServerName(serverNameBuffer, sizeof(serverNameBuffer));
	return jni->NewStringUTF(serverNameBuffer);
}

Java_Server_setSimple(jint, setPlayerLimit, SetMaxPlayers);
Java_Server_getSimple(jint, getPlayerLimit, GetMaxPlayers);

extern "C" JNIEXPORT void JNICALL Java_com_maxorator_vcmp_java_plugin_integration_server_ServerImpl_setServerPassword(JNIEnv* jni, jobject obj, jstring password) {
	if (!core->exc->ThreadCheck(jni)) return;

	api->SetServerPassword((char*)JavaString(jni, password).text);
}

extern "C" JNIEXPORT jstring JNICALL Java_com_maxorator_vcmp_java_plugin_integration_server_ServerImpl_getServerPassword(JNIEnv* jni, jobject obj) {
	if (!core->exc->ThreadCheck(jni)) return nullptr;

	char passwordBuffer[256];
	passwordBuffer[0] = '\0';

	api->GetServerPassword(passwordBuffer, sizeof(passwordBuffer));
	return jni->NewStringUTF(passwordBuffer);
}

extern "C" JNIEXPORT void JNICALL Java_com_maxorator_vcmp_java_plugin_integration_server_ServerImpl_setGameModeText(JNIEnv* jni, jobject obj, jstring text) {
	if (!core->exc->ThreadCheck(jni)) return;

	api->SetGameModeText((char*)JavaString(jni, text).text);
}

extern "C" JNIEXPORT jstring JNICALL Java_com_maxorator_vcmp_java_plugin_integration_server_ServerImpl_getGameModeText(JNIEnv* jni, jobject obj) {
	if (!core->exc->ThreadCheck(jni)) return nullptr;

	char textBuffer[256];
	textBuffer[0] = '\0';

	api->GetGameModeText(textBuffer, sizeof(textBuffer));
	return jni->NewStringUTF(textBuffer);
}

Java_Server_doSimple(shutdownServer, ShutdownServer);

extern "C" JNIEXPORT void JNICALL Java_com_maxorator_vcmp_java_plugin_integration_server_ServerImpl_setWorldBounds(JNIEnv* jni, jobject obj, jfloat maxX, jfloat minX, jfloat maxY, jfloat minY) {
	if (!core->exc->ThreadCheck(jni)) return;

	api->SetWorldBounds(maxX, minX, maxY, minY);
}

extern "C" JNIEXPORT jobject JNICALL Java_com_maxorator_vcmp_java_plugin_integration_server_ServerImpl_getWorldBounds(JNIEnv* jni, jobject obj) {
	if (!core->exc->ThreadCheck(jni)) return nullptr;

	float maxX = 0, minX = 0, maxY = 0, minY = 0;
	api->GetWorldBounds(&maxX, &minX, &maxY, &minY);
	return core->pools->CreateMapBounds(jni, maxX, minX, maxY, minY);
}

extern "C" JNIEXPORT void JNICALL Java_com_maxorator_vcmp_java_plugin_integration_server_ServerImpl_setWastedSettings(JNIEnv* jni, jobject obj, jint deathTimeMillis, jint fadeTimeMillis, jfloat fadeInSpeed, jfloat fadeOutSpeed, jint fadeColourHex, jint corpseFadeStart, jint corpseFadeDuration) {
	if (!core->exc->ThreadCheck(jni)) return;

	api->SetWastedSettings(deathTimeMillis, fadeTimeMillis, fadeInSpeed, fadeOutSpeed, fadeColourHex, corpseFadeStart, corpseFadeDuration);
}

extern "C" JNIEXPORT jobject JNICALL Java_com_maxorator_vcmp_java_plugin_integration_server_ServerImpl_getWastedSettings(JNIEnv* jni, jobject obj) {
	if (!core->exc->ThreadCheck(jni)) return nullptr;

	unsigned int deathTimeMillis = 0, fadeTimeMillis = 0, fadeColourHex = 0, corpseFadeStart = 0, corpseFadeDuration = 0;
	float fadeInSpeed = 0, fadeOutSpeed = 0;

	api->GetWastedSettings(&deathTimeMillis, &fadeTimeMillis, &fadeInSpeed, &fadeOutSpeed, &fadeColourHex, &corpseFadeStart, &corpseFadeDuration);
	return core->pools->CreateWastedSettings(jni, deathTimeMillis, fadeTimeMillis, fadeInSpeed, fadeOutSpeed, fadeColourHex, corpseFadeStart, corpseFadeDuration);
}

Java_Server_setSimple(jint, setTimeRate, SetTimeRate);
Java_Server_getSimple(jint, getTimeRate, GetTimeRate);
Java_Server_setSimple(jint, setHour, SetHour);
Java_Server_getSimple(jint, getHour, GetHour);
Java_Server_getSimple(jint, getFallTimer, GetFallTimer);
Java_Server_setSimple(jint, setFallTimer, SetFallTimer);
Java_Server_getSimple(jint, getMinute, GetMinute);
Java_Server_setSimple(jint, setWeather, SetWeather);
Java_Server_getSimple(jint, getWeather, GetWeather);
Java_Server_setSimple(jfloat, setGravity, SetGravity);
Java_Server_getSimple(jfloat, getGravity, GetGravity);
Java_Server_setSimple(jfloat, setGameSpeed, SetGameSpeed);
Java_Server_getSimple(jfloat, getGameSpeed, GetGameSpeed);
Java_Server_setSimple(jfloat, setWaterLevel, SetWaterLevel);
Java_Server_getSimple(jfloat, getWaterLevel, GetWaterLevel);
Java_Server_setSimple(jfloat, setAltitudeLimit, SetMaximumFlightAltitude);
Java_Server_getSimple(jfloat, getAltitudeLimit, GetMaximumFlightAltitude);
Java_Server_setSimple(jint, setKillCommandDelay, SetKillCommandDelay);
Java_Server_getSimple(jint, getKillCommandDelay, GetKillCommandDelay);
Java_Server_setSimple(jfloat, setVehiclesForcedRespawnAltitude, SetVehiclesForcedRespawnHeight);
Java_Server_getSimple(jfloat, getVehiclesForcedRespawnAltitude, GetVehiclesForcedRespawnHeight);

typedef int (*OPTS_ToggleServerOption) (unsigned int value);

extern "C" JNIEXPORT void JNICALL Java_com_maxorator_vcmp_java_plugin_integration_server_ServerImpl_setOption(JNIEnv* jni, jobject obj, jint option, jboolean value) {
	if (!core->exc->ThreadCheck(jni)) return;

	api->SetServerOption((vcmpServerOption)option, value);
}

typedef unsigned int(*OPTS_GetServerOption) (void);

extern "C" JNIEXPORT jboolean JNICALL Java_com_maxorator_vcmp_java_plugin_integration_server_ServerImpl_getOption(JNIEnv* jni, jobject obj, jint option) {
	if (!core->exc->ThreadCheck(jni)) return false;

	return api->GetServerOption((vcmpServerOption)option);
}

extern "C" JNIEXPORT void JNICALL Java_com_maxorator_vcmp_java_plugin_integration_server_ServerImpl_createExplosion(JNIEnv* jni, jobject obj,
		jint worldId, jint type, jfloat x, jfloat y, jfloat z, jobject responsiblePlayer, jboolean atGroundLevel) {
	if (!core->exc->ThreadCheck(jni)) return;

	api->CreateExplosion(worldId, type, x, y, z, core->pools->players->GetId(jni, responsiblePlayer), atGroundLevel);
}

extern "C" JNIEXPORT void JNICALL Java_com_maxorator_vcmp_java_plugin_integration_server_ServerImpl_playSound(JNIEnv* jni, jobject obj,
		jint worldId, jint soundId, jfloat x, jfloat y, jfloat z) {
	if (!core->exc->ThreadCheck(jni)) return;

	api->PlaySound(worldId, soundId, x, y, z);
}

extern "C" JNIEXPORT void JNICALL Java_com_maxorator_vcmp_java_plugin_integration_server_ServerImpl_hideMapObject(JNIEnv* jni, jobject obj, jint modelId, jfloat x, jfloat y, jfloat z) {
	if (!core->exc->ThreadCheck(jni)) return;

	api->HideMapObject(modelId, (int)(x * 10.0f + 0.5f), (int)(y * 10.0f + 0.5f), (int)(z * 10.0f + 0.5f));
}

extern "C" JNIEXPORT void JNICALL Java_com_maxorator_vcmp_java_plugin_integration_server_ServerImpl_showMapObject(JNIEnv* jni, jobject obj, jint modelId, jfloat x, jfloat y, jfloat z) {
	if (!core->exc->ThreadCheck(jni)) return;

	api->ShowMapObject(modelId, (int)(x * 10.0f + 0.5f), (int)(y * 10.0f + 0.5f), (int)(z * 10.0f + 0.5f));
}

Java_Server_doSimple(showAllMapObjects, ShowAllMapObjects);

extern "C" JNIEXPORT void JNICALL Java_com_maxorator_vcmp_java_plugin_integration_server_ServerImpl_setWeaponDataValue(JNIEnv* jni, jobject obj, jint weaponId, jint fieldId, jdouble value) {
	if (!core->exc->ThreadCheck(jni)) return;

	api->SetWeaponDataValue(weaponId, fieldId, value);
}

extern "C" JNIEXPORT jdouble JNICALL Java_com_maxorator_vcmp_java_plugin_integration_server_ServerImpl_getWeaponDataValue(JNIEnv* jni, jobject obj, jint weaponId, jint fieldId) {
	if (!core->exc->ThreadCheck(jni)) return 0;

	return api->GetWeaponDataValue(weaponId, fieldId);
}

extern "C" JNIEXPORT void JNICALL Java_com_maxorator_vcmp_java_plugin_integration_server_ServerImpl_resetWeaponDataValue(JNIEnv* jni, jobject obj, jint weaponId, jint fieldId) {
	if (!core->exc->ThreadCheck(jni)) return;

	api->ResetWeaponDataValue(weaponId, fieldId);
}

extern "C" JNIEXPORT jboolean JNICALL Java_com_maxorator_vcmp_java_plugin_integration_server_ServerImpl_isWeaponDataValueModified(JNIEnv* jni, jobject obj, jint weaponId, jint fieldId) {
	if (!core->exc->ThreadCheck(jni)) return false;

	return api->IsWeaponDataValueModified(weaponId, fieldId);
}

Java_Server_setSimple(jint, resetWeaponData, ResetWeaponData);
Java_Server_doSimple(resetAllWeaponData, ResetAllWeaponData);

Java_Server_getSimple(jint, getUnusedKeybindSlot, GetKeyBindUnusedSlot);

extern "C" JNIEXPORT jobject JNICALL Java_com_maxorator_vcmp_java_plugin_integration_server_ServerImpl_getKeyBind(JNIEnv* jni, jobject obj, jint keyBindId) {
	if (!core->exc->ThreadCheck(jni)) return nullptr;

	int keyOne, keyTwo, keyThree;
	uint8_t onRelease;

	if (!api->GetKeyBindData(keyBindId, &onRelease, &keyOne, &keyTwo, &keyThree)) {
		return nullptr;
	}

	return core->pools->CreateKeyBind(jni, keyBindId, onRelease ? true : false, keyOne, keyTwo, keyThree);
}

extern "C" JNIEXPORT jboolean JNICALL Java_com_maxorator_vcmp_java_plugin_integration_server_ServerImpl_registerKeyBind(JNIEnv* jni, jobject obj,
		jint keyBindId, jboolean onRelease, jint keyOne, jint keyTwo, jint keyThree) {
	if (!core->exc->ThreadCheck(jni)) return false;

	return api->RegisterKeyBind(keyBindId, onRelease, keyOne, keyTwo, keyThree);
}

extern "C" JNIEXPORT jboolean JNICALL Java_com_maxorator_vcmp_java_plugin_integration_server_ServerImpl_removeKeyBind(JNIEnv* jni, jobject obj, jint keyBindId) {
	if (!core->exc->ThreadCheck(jni)) return false;

	return api->RemoveKeyBind(keyBindId);
}

Java_Server_doSimple(removeAllKeyBinds, RemoveAllKeyBinds);

extern "C" JNIEXPORT jint JNICALL Java_com_maxorator_vcmp_java_plugin_integration_server_ServerImpl_createCoordBlip(JNIEnv* jni, jobject obj,
		jint index, jint worldId, jfloat x, jfloat y, jfloat z, jint scale, jint colourHex, jint spriteId) {
	if (!core->exc->ThreadCheck(jni)) return -1;

	return api->CreateCoordBlip(index, worldId, x, y, z, scale, Common::ConvertArgbToRgba((uint32_t)colourHex), spriteId);
}

Java_Server_setSimple(jint, destroyCoordBlip, DestroyCoordBlip);

extern "C" JNIEXPORT jobject JNICALL Java_com_maxorator_vcmp_java_plugin_integration_server_ServerImpl_getCoordBlipInfo(JNIEnv* jni, jobject obj, jint index) {
	if (!core->exc->ThreadCheck(jni)) return nullptr;

	int worldId, scale, spriteId;
	uint32_t colour;
	float x, y, z;

	if (!api->GetCoordBlipInfo(index, &worldId, &x, &y, &z, &scale, &colour, &spriteId)) {
		return nullptr;
	}

	return core->pools->CreateCoordBlipInfo(jni, index, worldId, x, y, z, scale, Common::ConvertRgbaToArgb(colour), spriteId);
}

extern "C" JNIEXPORT void JNICALL Java_com_maxorator_vcmp_java_plugin_integration_server_ServerImpl_addRadioStream(JNIEnv* jni, jobject obj, jint index, jstring name, jstring url, jboolean isListed) {
	if (!core->exc->ThreadCheck(jni)) return;

	api->AddRadioStream(index, JavaString(jni, name).text, JavaString(jni, url).text, isListed);
}

Java_Server_setSimple(jint, removeRadioStream, RemoveRadioStream);

extern "C" JNIEXPORT jint JNICALL Java_com_maxorator_vcmp_java_plugin_integration_server_ServerImpl_addPlayerClass(JNIEnv* jni, jobject obj,
		jint teamId, jint colour, jint modelId, jfloat x, jfloat y, jfloat z, jfloat angle, jint weaponOne, jint ammoOne, jint weaponTwo, jint ammoTwo, jint weaponThree, jint ammoThree) {
	if (!core->exc->ThreadCheck(jni)) return -1;

	return api->AddPlayerClass(teamId, colour, modelId, x, y, z, angle, weaponOne, ammoOne, weaponTwo, ammoTwo, weaponThree, ammoThree);
}

extern "C" JNIEXPORT void JNICALL Java_com_maxorator_vcmp_java_plugin_integration_server_ServerImpl_setSpawnScreenPlayerPos(JNIEnv* jni, jobject obj, jfloat x, jfloat y, jfloat z) {
	if (!core->exc->ThreadCheck(jni)) return;

	api->SetSpawnPlayerPosition(x, y, z);
}

extern "C" JNIEXPORT void JNICALL Java_com_maxorator_vcmp_java_plugin_integration_server_ServerImpl_setSpawnScreenCameraPos(JNIEnv* jni, jobject obj, jfloat x, jfloat y, jfloat z) {
	if (!core->exc->ThreadCheck(jni)) return;

	api->SetSpawnCameraPosition(x, y, z);
}

extern "C" JNIEXPORT void JNICALL Java_com_maxorator_vcmp_java_plugin_integration_server_ServerImpl_setSpawnScreenCameraLookAt(JNIEnv* jni, jobject obj, jfloat x, jfloat y, jfloat z) {
	if (!core->exc->ThreadCheck(jni)) return;

	api->SetSpawnCameraLookAt(x, y, z);
}

extern "C" JNIEXPORT void JNICALL Java_com_maxorator_vcmp_java_plugin_integration_server_ServerImpl_banAddress(JNIEnv* jni, jobject obj, jstring ip) {
	if (!core->exc->ThreadCheck(jni)) return;

	api->BanIP((char*)JavaString(jni, ip).text);
}

extern "C" JNIEXPORT void JNICALL Java_com_maxorator_vcmp_java_plugin_integration_server_ServerImpl_unbanAddress(JNIEnv* jni, jobject obj, jstring ip) {
	if (!core->exc->ThreadCheck(jni)) return;

	api->UnbanIP((char*)JavaString(jni, ip).text);
}

extern "C" JNIEXPORT jboolean JNICALL Java_com_maxorator_vcmp_java_plugin_integration_server_ServerImpl_isAddressBanned(JNIEnv* jni, jobject obj, jstring ip) {
	if (!core->exc->ThreadCheck(jni)) return false;

	return api->IsIPBanned((char*)JavaString(jni, ip).text);
}

extern "C" JNIEXPORT jobject JNICALL Java_com_maxorator_vcmp_java_plugin_integration_server_ServerImpl_getPlayer(JNIEnv* jni, jobject obj, jint index) {
	if (!core->exc->ThreadCheck(jni)) return nullptr;

	return core->pools->players->Get(jni, index);
}

extern "C" JNIEXPORT jobjectArray JNICALL Java_com_maxorator_vcmp_java_plugin_integration_server_ServerImpl_getAllPlayers(JNIEnv* jni, jobject obj) {
	if (!core->exc->ThreadCheck(jni)) return nullptr;

	return core->pools->players->GetAll(jni);
}

extern "C" JNIEXPORT jobject JNICALL Java_com_maxorator_vcmp_java_plugin_integration_server_ServerImpl_findPlayer(JNIEnv* jni, jobject obj, jstring name) {
	if (!core->exc->ThreadCheck(jni)) return nullptr;

	return core->pools->players->Get(jni, api->GetPlayerIdFromName((char*)JavaString(jni, name).text));
}

extern "C" JNIEXPORT jobject JNICALL Java_com_maxorator_vcmp_java_plugin_integration_server_ServerImpl_createVehicle(JNIEnv* jni, jobject obj,
		jint modelId, jint worldId, jfloat x, jfloat y, jfloat z, jfloat angle, jint primary, jint secondary) {

	if (!core->exc->ThreadCheck(jni)) return nullptr;

	int vehicleId = api->CreateVehicle(modelId, worldId, x, y, z, angle, primary, secondary);

	if (vehicleId <= 0) {
		return nullptr;
	}

	return core->pools->vehicles->Get(jni, vehicleId);
}

extern "C" JNIEXPORT jobject JNICALL Java_com_maxorator_vcmp_java_plugin_integration_server_ServerImpl_getVehicle(JNIEnv* jni, jobject obj, jint index) {
	if (!core->exc->ThreadCheck(jni)) return nullptr;

	if (!api->CheckEntityExists(vcmpEntityPoolVehicle, index)) {
		return nullptr;
	}

	return core->pools->vehicles->Get(jni, index);
}

extern "C" JNIEXPORT jobjectArray JNICALL Java_com_maxorator_vcmp_java_plugin_integration_server_ServerImpl_getAllVehicles(JNIEnv* jni, jobject obj) {
	if (!core->exc->ThreadCheck(jni)) return nullptr;

	return core->pools->vehicles->GetAll(jni);
}

Java_Server_doSimple(resetAllVehiclehandlings, ResetAllVehicleHandlings);

extern "C" JNIEXPORT jboolean JNICALL Java_com_maxorator_vcmp_java_plugin_integration_server_ServerImpl_handlingRuleExists(JNIEnv* jni, jobject obj, jint model, jint rule) {
	if (!core->exc->ThreadCheck(jni)) return false;

	return api->ExistsHandlingRule(model, rule);
}

extern "C" JNIEXPORT void JNICALL Java_com_maxorator_vcmp_java_plugin_integration_server_ServerImpl_setHandlingRule(JNIEnv* jni, jobject obj, jint model, jint rule, jdouble value) {
	if (!core->exc->ThreadCheck(jni)) return;

	api->SetHandlingRule(model, rule, value);
}

extern "C" JNIEXPORT jdouble JNICALL Java_com_maxorator_vcmp_java_plugin_integration_server_ServerImpl_getHandlingRule(JNIEnv* jni, jobject obj, jint model, jint rule) {
	if (!core->exc->ThreadCheck(jni)) return 0;

	return api->GetHandlingRule(model, rule);
}

extern "C" JNIEXPORT void JNICALL Java_com_maxorator_vcmp_java_plugin_integration_server_ServerImpl_resetHandlingRule(JNIEnv* jni, jobject obj, jint model, jint rule) {
	if (!core->exc->ThreadCheck(jni)) return;

	api->ResetHandlingRule(model, rule);
}

Java_Server_setSimple(jint, resetHandling, ResetHandling);

extern "C" JNIEXPORT jobject JNICALL Java_com_maxorator_vcmp_java_plugin_integration_server_ServerImpl_getPickup(JNIEnv* jni, jobject obj, jint index) {
	if (!core->exc->ThreadCheck(jni)) return nullptr;

	if (!api->CheckEntityExists(vcmpEntityPoolPickup, index)) {
		return nullptr;
	}

	return core->pools->pickups->Get(jni, index);
}

extern "C" JNIEXPORT jobject JNICALL Java_com_maxorator_vcmp_java_plugin_integration_server_ServerImpl_createPickup(JNIEnv* jni, jobject obj,
		jint modelId, jint worldId, jint quantity, jfloat x, jfloat y, jfloat z, jint alpha, jboolean automatic) {
	if (!core->exc->ThreadCheck(jni)) return nullptr;

	int pickupId = api->CreatePickup(modelId, worldId, quantity, x, y, z, alpha, automatic);

	if (pickupId < 0) {
		return nullptr;
	}

	return core->pools->pickups->Get(jni, pickupId);
}

extern "C" JNIEXPORT jobject JNICALL Java_com_maxorator_vcmp_java_plugin_integration_server_ServerImpl_getObject(JNIEnv* jni, jobject obj, jint index) {
	if (!core->exc->ThreadCheck(jni)) return nullptr;

	if (!api->CheckEntityExists(vcmpEntityPoolObject, index)) {
		return nullptr;
	}

	return core->pools->objects->Get(jni, index);
}

extern "C" JNIEXPORT jobject JNICALL Java_com_maxorator_vcmp_java_plugin_integration_server_ServerImpl_createObject(JNIEnv* jni, jobject obj,
		jint modelId, jint worldId, jfloat x, jfloat y, jfloat z, jint alpha) {
	if (!core->exc->ThreadCheck(jni)) return nullptr;

	int objectId = api->CreateObject(modelId, worldId, x, y, z, alpha);

	if (objectId < 0) {
		return nullptr;
	}

	return core->pools->objects->Get(jni, objectId);
}

extern "C" JNIEXPORT jobject JNICALL Java_com_maxorator_vcmp_java_plugin_integration_server_ServerImpl_getCheckPoint(JNIEnv* jni, jobject obj, jint index) {
	if (!core->exc->ThreadCheck(jni)) return nullptr;

	if (!api->CheckEntityExists(vcmpEntityPoolCheckPoint, index)) {
		return nullptr;
	}

	return core->pools->checkpoints->Get(jni, index);
}

extern "C" JNIEXPORT jobject JNICALL Java_com_maxorator_vcmp_java_plugin_integration_server_ServerImpl_createCheckPoint(JNIEnv* jni, jobject obj,
		jobject player, jint worldId, jboolean isSphere, jfloat x, jfloat y, jfloat z, jint r, jint g, jint b, jint a, jfloat radius) {
	int playerId = -1;

	if (!core->exc->ThreadCheck(jni)) return nullptr;
	else if (player != nullptr && (playerId = core->pools->players->GetId(jni, player)) == -1) return nullptr;

	int checkPointId = api->CreateCheckPoint(playerId, worldId, isSphere, x, y, z, r, g, b, a, radius);

	return core->pools->checkpoints->Get(jni, checkPointId);
}
