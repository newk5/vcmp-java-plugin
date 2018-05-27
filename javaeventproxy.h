#pragma once
#include "plugin.h"

class JavaEventProxy {
public:
	static void MapEvents(void);
	static void UnmapEvents(void);

	static bool StartVM(PluginCallbacks* pluginEvents);
	static void FlagForRestart(void);

private:
	static PluginCallbacks* m_pluginEvents;
	static bool m_restartTriggered;
	static bool m_isInLimbo;
	static bool m_isAlive;

	static void ShutdownVM(void);
	static void RestartVM(void);
	static void ReloadBindings(void);

	static uint8_t OnServerInitialise(void);
	static void OnServerShutdown(void);
	static void OnServerFrame(float elapsedTime);
	static uint8_t OnPluginCommand(uint32_t commandIdentifier, const char* message);
	static uint8_t OnIncomingConnection(char* playerName, size_t nameBufferSize, const char* userPassword, const char* ipAddress);
	static void OnClientScriptData(int32_t playerId, const uint8_t* data, size_t size);
	static void OnPlayerConnect(int32_t playerId);
	static void OnPlayerDisconnect(int32_t playerId, vcmpDisconnectReason reason);
	static uint8_t OnPlayerRequestClass(int32_t playerId, int32_t offset);
	static uint8_t OnPlayerRequestSpawn(int32_t playerId);
	static void OnPlayerSpawn(int32_t playerId);
	static void OnPlayerDeath(int32_t playerId, int32_t killerId, int32_t reason, vcmpBodyPart bodyPart);
	static void OnPlayerUpdate(int32_t playerId, vcmpPlayerUpdate updateType);
	static uint8_t OnPlayerRequestEnterVehicle(int32_t playerId, int32_t vehicleId, int32_t slotIndex);
	static void OnPlayerEnterVehicle(int32_t playerId, int32_t vehicleId, int32_t slotIndex);
	static void OnPlayerExitVehicle(int32_t playerId, int32_t vehicleId);
	static void OnPlayerNameChange(int32_t playerId, const char* oldName, const char* newName);
	static void OnPlayerStateChange(int32_t playerId, vcmpPlayerState oldState, vcmpPlayerState newState);
	static void OnPlayerActionChange(int32_t playerId, int32_t oldAction, int32_t newAction);
	static void OnPlayerOnFireChange(int32_t playerId, uint8_t isOnFire);
	static void OnPlayerCrouchChange(int32_t playerId, uint8_t isCrouching);
	static void OnPlayerGameKeysChange(int32_t playerId, uint32_t oldKeys, uint32_t newKeys);
	static void OnPlayerBeginTyping(int32_t playerId);
	static void OnPlayerEndTyping(int32_t playerId);
	static void OnPlayerAwayChange(int32_t playerId, uint8_t isAway);
	static uint8_t OnPlayerMessage(int32_t playerId, const char* message);
	static uint8_t OnPlayerCommand(int32_t playerId, const char* message);
	static uint8_t OnPlayerPrivateMessage(int32_t playerId, int32_t targetPlayerId, const char* message);
	static void OnPlayerKeyBindDown(int32_t playerId, int32_t bindId);
	static void OnPlayerKeyBindUp(int32_t playerId, int32_t bindId);
	static void OnPlayerSpectate(int32_t playerId, int32_t targetPlayerId);
	static void OnPlayerCrashReport(int32_t playerId, const char* report);
	static void OnVehicleUpdate(int32_t vehicleId, vcmpVehicleUpdate updateType);
	static void OnVehicleExplode(int32_t vehicleId);
	static void OnVehicleRespawn(int32_t vehicleId);
	static void OnObjectShot(int32_t objectId, int32_t playerId, int32_t weaponId);
	static void OnObjectTouched(int32_t objectId, int32_t playerId);
	static uint8_t OnPickupPickAttempt(int32_t pickupId, int32_t playerId);
	static void OnPickupPicked(int32_t pickupId, int32_t playerId);
	static void OnPickupRespawn(int32_t pickupId);
	static void OnCheckpointEntered(int32_t checkPointId, int32_t playerId);
	static void OnCheckpointExited(int32_t checkPointId, int32_t playerId);
	static void OnEntityPoolChange(vcmpEntityPool entityType, int32_t entityId, uint8_t isDeleted);
	static void OnServerPerformanceReport(size_t entryCount, const char** descriptions, uint64_t* times);
};
