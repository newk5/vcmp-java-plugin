#pragma once
#include <stdint.h>
#include "plugin.h"

class JavaEventHandler {
public:
	JavaEventHandler(void);
	~JavaEventHandler(void);

	bool BindToClasses(bool isFirstBinding, int64_t flags, jobject existingEventsInstance);

	void OnLoadScripts(void);
	void OnUnloadScripts(void);
	bool OnServerInitialise(void);
	void OnServerShutdown(void);
	void OnServerFrame(float elapsedTime);
	bool OnPluginCommand(uint32_t commandIdentifier, const char* message);
	bool OnIncomingConnection(char* playerName, size_t nameBufferSize, const char* userPassword, const char* ipAddress);
	void OnClientScriptData(int32_t playerId, const uint8_t* data, size_t size);
	void OnPlayerConnect(int32_t playerId);
	void OnPlayerDisconnect(int32_t playerId, vcmpDisconnectReason reason);
	bool OnPlayerRequestClass(int32_t playerId, int32_t offset);
	bool OnPlayerRequestSpawn(int32_t playerId);
	void OnPlayerSpawn(int32_t playerId);
	void OnPlayerDeath(int32_t playerId, int32_t killerId, int32_t reason, vcmpBodyPart bodyPart);
	void OnPlayerUpdate(int32_t playerId, vcmpPlayerUpdate updateType);
	bool OnPlayerRequestEnterVehicle(int32_t playerId, int32_t vehicleId, int32_t slotIndex);
	void OnPlayerEnterVehicle(int32_t playerId, int32_t vehicleId, int32_t slotIndex);
	void OnPlayerExitVehicle(int32_t playerId, int32_t vehicleId);
	void OnPlayerNameChange(int32_t playerId, const char* oldName, const char* newName);
	void OnPlayerStateChange(int32_t playerId, vcmpPlayerState oldState, vcmpPlayerState newState);
	void OnPlayerActionChange(int32_t playerId, int32_t oldAction, int32_t newAction);
	void OnPlayerOnFireChange(int32_t playerId, bool isOnFire);
	void OnPlayerCrouchChange(int32_t playerId, bool isCrouching);
	void OnPlayerGameKeysChange(int32_t playerId, uint32_t oldKeys, uint32_t newKeys);
	void OnPlayerBeginTyping(int32_t playerId);
	void OnPlayerEndTyping(int32_t playerId);
	void OnPlayerAwayChange(int32_t playerId, bool isAway);
	bool OnPlayerMessage(int32_t playerId, const char* message);
	bool OnPlayerCommand(int32_t playerId, const char* message);
	bool OnPlayerPrivateMessage(int32_t playerId, int32_t targetPlayerId, const char* message);
	void OnPlayerKeyBindDown(int32_t playerId, int32_t bindId);
	void OnPlayerKeyBindUp(int32_t playerId, int32_t bindId);
	void OnPlayerSpectate(int32_t playerId, int32_t targetPlayerId);
	void OnPlayerCrashReport(int32_t playerId, const char* report);
	void OnVehicleUpdate(int32_t vehicleId, vcmpVehicleUpdate updateType);
	void OnVehicleExplode(int32_t vehicleId);
	void OnVehicleRespawn(int32_t vehicleId);
	void OnObjectShot(int32_t objectId, int32_t playerId, int32_t weaponId);
	void OnObjectTouched(int32_t objectId, int32_t playerId);
	bool OnPickupPickAttempt(int32_t pickupId, int32_t playerId);
	void OnPickupPicked(int32_t pickupId, int32_t playerId);
	void OnPickupRespawn(int32_t pickupId);
	void OnCheckpointEntered(int32_t checkPointId, int32_t playerId);
	void OnCheckpointExited(int32_t checkPointId, int32_t playerId);
	void OnEntityPoolChange(vcmpEntityPool entityType, int32_t entityId, bool isDeleted);
	void OnServerPerformanceReport(size_t entryCount, const char** descriptions, uint64_t* times);

private:
	jmethodID GetOwnMethod(jclass klass, int64_t flagMatch, const char* name, const char* signature);
	jmethodID FilterOwnMethod(jmethodID methodId, jclass match);

	jclass eventsClass;
	jobject eventsInstance;
	jclass serverClass;
	jobject serverInstance;

	jmethodID onLoadScriptsMethod;
	jmethodID unUnloadScriptsMethod;
	jmethodID onServerInitialiseMethod;
	jmethodID onServerShutdownMethod;
	jmethodID onServerFrameMethod;
	jmethodID onPluginCommandMethod;
	jmethodID onIncomingConnectionMethod;
	jmethodID onClientScriptDataMethod;
	jmethodID onPlayerConnectMethod;
	jmethodID onPlayerDisconnectMethod;
	jmethodID onPlayerRequestClassMethod;
	jmethodID onPlayerRequestSpawnMethod;
	jmethodID onPlayerSpawnMethod;
	jmethodID onPlayerDeathMethod;
	jmethodID onPlayerUpdateMethod;
	jmethodID onPlayerRequestEnterVehicleMethod;
	jmethodID onPlayerEnterVehicleMethod;
	jmethodID onPlayerExitVehicleMethod;
	jmethodID onPlayerNameChangeMethod;
	jmethodID onPlayerStateChangeMethod;
	jmethodID onPlayerActionChangeMethod;
	jmethodID onPlayerOnFireChangeMethod;
	jmethodID onPlayerCrouchChangeMethod;
	jmethodID onPlayerGameKeysChangeMethod;
	jmethodID onPlayerBeginTypingMethod;
	jmethodID onPlayerEndTypingMethod;
	jmethodID onPlayerAwayChangeMethod;
	jmethodID onPlayerMessageMethod;
	jmethodID onPlayerCommandMethod;
	jmethodID onPlayerPrivateMessageMethod;
	jmethodID onPlayerKeyBindDownMethod;
	jmethodID onPlayerKeyBindUpMethod;
	jmethodID onPlayerSpectateMethod;
	jmethodID onPlayerCrashReportMethod;
	jmethodID onVehicleUpdateMethod;
	jmethodID onVehicleExplodeMethod;
	jmethodID onVehicleRespawnMethod;
	jmethodID onObjectShotMethod;
	jmethodID onObjectTouchedMethod;
	jmethodID onPickupPickAttemptMethod;
	jmethodID onPickupPickedMethod;
	jmethodID onPickupRespawnMethod;
	jmethodID onCheckpointEnteredMethod;
	jmethodID onCheckpointExitedMethod;
};
