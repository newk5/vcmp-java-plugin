#include "javaeventproxy.h"
#include "main.h"
#include "logging.h"
#include <string.h>

PluginCallbacks* JavaEventProxy::m_pluginEvents;
bool JavaEventProxy::m_restartTriggered;
bool JavaEventProxy::m_isAlive;
bool JavaEventProxy::m_isInLimbo;

void JavaEventProxy::MapEvents(void) {
	m_pluginEvents->OnServerInitialise = OnServerInitialise;
	m_pluginEvents->OnServerShutdown = OnServerShutdown;
	m_pluginEvents->OnServerFrame = OnServerFrame;

	m_pluginEvents->OnPluginCommand = OnPluginCommand;
	m_pluginEvents->OnIncomingConnection = OnIncomingConnection;
	m_pluginEvents->OnClientScriptData = OnClientScriptData;

	m_pluginEvents->OnPlayerConnect = OnPlayerConnect;
	m_pluginEvents->OnPlayerDisconnect = OnPlayerDisconnect;

	m_pluginEvents->OnPlayerRequestClass = OnPlayerRequestClass;
	m_pluginEvents->OnPlayerRequestSpawn = OnPlayerRequestSpawn;
	m_pluginEvents->OnPlayerSpawn = OnPlayerSpawn;
	m_pluginEvents->OnPlayerDeath = OnPlayerDeath;
	m_pluginEvents->OnPlayerUpdate = OnPlayerUpdate;
	
	m_pluginEvents->OnPlayerRequestEnterVehicle = OnPlayerRequestEnterVehicle;
	m_pluginEvents->OnPlayerEnterVehicle = OnPlayerEnterVehicle;
	m_pluginEvents->OnPlayerExitVehicle = OnPlayerExitVehicle;

	m_pluginEvents->OnPlayerNameChange = OnPlayerNameChange;
	m_pluginEvents->OnPlayerStateChange = OnPlayerStateChange;
	m_pluginEvents->OnPlayerActionChange = OnPlayerActionChange;
	m_pluginEvents->OnPlayerOnFireChange = OnPlayerOnFireChange;
	m_pluginEvents->OnPlayerCrouchChange = OnPlayerCrouchChange;
	m_pluginEvents->OnPlayerGameKeysChange = OnPlayerGameKeysChange;
	m_pluginEvents->OnPlayerBeginTyping = OnPlayerBeginTyping;
	m_pluginEvents->OnPlayerEndTyping = OnPlayerEndTyping;
	m_pluginEvents->OnPlayerAwayChange = OnPlayerAwayChange;

	m_pluginEvents->OnPlayerMessage = OnPlayerMessage;
	m_pluginEvents->OnPlayerCommand = OnPlayerCommand;
	m_pluginEvents->OnPlayerPrivateMessage = OnPlayerPrivateMessage;

	m_pluginEvents->OnPlayerKeyBindDown = OnPlayerKeyBindDown;
	m_pluginEvents->OnPlayerKeyBindUp = OnPlayerKeyBindUp;
	m_pluginEvents->OnPlayerSpectate = OnPlayerSpectate;
	m_pluginEvents->OnPlayerCrashReport = OnPlayerCrashReport;

	m_pluginEvents->OnVehicleUpdate = OnVehicleUpdate;
	m_pluginEvents->OnVehicleExplode = OnVehicleExplode;
	m_pluginEvents->OnVehicleRespawn = OnVehicleRespawn;

	m_pluginEvents->OnObjectShot = OnObjectShot;
	m_pluginEvents->OnObjectTouched = OnObjectTouched;

	m_pluginEvents->OnPickupPickAttempt = OnPickupPickAttempt;
	m_pluginEvents->OnPickupPicked = OnPickupPicked;
	m_pluginEvents->OnPickupRespawn = OnPickupRespawn;

	m_pluginEvents->OnCheckpointEntered = OnCheckpointEntered;
	m_pluginEvents->OnCheckpointExited = OnCheckpointExited;

	m_pluginEvents->OnEntityPoolChange = OnEntityPoolChange;
	m_pluginEvents->OnServerPerformanceReport = OnServerPerformanceReport;

	m_isInLimbo = false;
}

void JavaEventProxy::UnmapEvents(void) {
	memset(m_pluginEvents, 0, sizeof(*m_pluginEvents));

	m_isInLimbo = true;

	m_pluginEvents->OnServerFrame = OnServerFrame;
}

bool JavaEventProxy::StartVM(PluginCallbacks* pluginEvents) {
	m_pluginEvents = pluginEvents;

	core = new JavaCore();
	if (!core->Initialise()) {
		Logging::Log(LogError, __FUNCTION__, "JavaCore initialisation failed.");
		return false;
	}

	MapEvents();

	if (m_isAlive) {
		core->eventHandler->OnLoadScripts();
	}

	return true;
}

void JavaEventProxy::FlagForRestart(void) {
	m_restartTriggered = true;
}

void JavaEventProxy::ShutdownVM(void) {
	core->eventHandler->OnUnloadScripts();

	UnmapEvents();
	delete core;
}

void JavaEventProxy::RestartVM(void) {
	m_restartTriggered = false;

	core->eventHandler->OnUnloadScripts();
	UnmapEvents();

	core->Unload();

	Logging::Log(LogInfo, __FUNCTION__, "Script context unloaded.");

	ReloadBindings();
}

void JavaEventProxy::ReloadBindings(void) {
	if (core->Reload()) {
		Logging::Log(LogInfo, __FUNCTION__, "Reloading script bindings.");

		MapEvents();
		core->eventHandler->OnLoadScripts();
	}
}

uint8_t JavaEventProxy::OnServerInitialise(void) {
	m_isAlive = true;

	return core->eventHandler->OnServerInitialise();
}


void JavaEventProxy::OnServerShutdown(void) {
	core->eventHandler->OnServerShutdown();

	ShutdownVM();
}


void JavaEventProxy::OnServerFrame(float elapsedTime) {
	if (m_isInLimbo) {
		ReloadBindings();
	}
	else if (m_restartTriggered) {
		RestartVM();
	}
	else {
		core->eventHandler->OnServerFrame(elapsedTime);
	}
}


uint8_t JavaEventProxy::OnPluginCommand(uint32_t commandIdentifier, const char* message) {
	return core->eventHandler->OnPluginCommand(commandIdentifier, message) ? 1 : 0;
}


uint8_t JavaEventProxy::OnIncomingConnection(char* playerName, size_t nameBufferSize, const char* userPassword, const char* ipAddress) {
	return core->eventHandler->OnIncomingConnection(playerName, nameBufferSize, userPassword, ipAddress) ? 1 : 0;
}


void JavaEventProxy::OnClientScriptData(int32_t playerId, const uint8_t* data, size_t size) {
	core->eventHandler->OnClientScriptData(playerId, data, size);
}


void JavaEventProxy::OnPlayerConnect(int32_t playerId) {
	core->eventHandler->OnPlayerConnect(playerId);
}


void JavaEventProxy::OnPlayerDisconnect(int32_t playerId, vcmpDisconnectReason reason) {
	core->eventHandler->OnPlayerDisconnect(playerId, reason);
}


uint8_t JavaEventProxy::OnPlayerRequestClass(int32_t playerId, int32_t offset) {
	return core->eventHandler->OnPlayerRequestClass(playerId, offset) ? 1 : 0;
}


uint8_t JavaEventProxy::OnPlayerRequestSpawn(int32_t playerId) {
	return core->eventHandler->OnPlayerRequestSpawn(playerId) ? 1 : 0;
}


void JavaEventProxy::OnPlayerSpawn(int32_t playerId) {
	core->eventHandler->OnPlayerSpawn(playerId);
}


void JavaEventProxy::OnPlayerDeath(int32_t playerId, int32_t killerId, int32_t reason, vcmpBodyPart bodyPart) {
	core->eventHandler->OnPlayerDeath(playerId, killerId, reason, bodyPart);
}


void JavaEventProxy::OnPlayerUpdate(int32_t playerId, vcmpPlayerUpdate updateType) {
	core->eventHandler->OnPlayerUpdate(playerId, updateType);
}


uint8_t JavaEventProxy::OnPlayerRequestEnterVehicle(int32_t playerId, int32_t vehicleId, int32_t slotIndex) {
	return core->eventHandler->OnPlayerRequestEnterVehicle(playerId, vehicleId, slotIndex) ? 1 : 0;
}


void JavaEventProxy::OnPlayerEnterVehicle(int32_t playerId, int32_t vehicleId, int32_t slotIndex) {
	core->eventHandler->OnPlayerEnterVehicle(playerId, vehicleId, slotIndex);
}


void JavaEventProxy::OnPlayerExitVehicle(int32_t playerId, int32_t vehicleId) {
	core->eventHandler->OnPlayerExitVehicle(playerId, vehicleId);
}


void JavaEventProxy::OnPlayerNameChange(int32_t playerId, const char* oldName, const char* newName) {
	core->eventHandler->OnPlayerNameChange(playerId, oldName, newName);
}


void JavaEventProxy::OnPlayerStateChange(int32_t playerId, vcmpPlayerState oldState, vcmpPlayerState newState) {
	core->eventHandler->OnPlayerStateChange(playerId, oldState, newState);
}


void JavaEventProxy::OnPlayerActionChange(int32_t playerId, int32_t oldAction, int32_t newAction) {
	core->eventHandler->OnPlayerActionChange(playerId, oldAction, newAction);
}


void JavaEventProxy::OnPlayerOnFireChange(int32_t playerId, uint8_t isOnFire) {
	core->eventHandler->OnPlayerOnFireChange(playerId, !!isOnFire);
}


void JavaEventProxy::OnPlayerCrouchChange(int32_t playerId, uint8_t isCrouching) {
	core->eventHandler->OnPlayerCrouchChange(playerId, !!isCrouching);
}


void JavaEventProxy::OnPlayerGameKeysChange(int32_t playerId, uint32_t oldKeys, uint32_t newKeys) {
	core->eventHandler->OnPlayerGameKeysChange(playerId, oldKeys, newKeys);
}


void JavaEventProxy::OnPlayerBeginTyping(int32_t playerId) {
	core->eventHandler->OnPlayerBeginTyping(playerId);
}


void JavaEventProxy::OnPlayerEndTyping(int32_t playerId) {
	core->eventHandler->OnPlayerEndTyping(playerId);
}


void JavaEventProxy::OnPlayerAwayChange(int32_t playerId, uint8_t isAway) {
	core->eventHandler->OnPlayerAwayChange(playerId, !!isAway);
}


uint8_t JavaEventProxy::OnPlayerMessage(int32_t playerId, const char* message) {
	return core->eventHandler->OnPlayerMessage(playerId, message) ? 1 : 0;
}


uint8_t JavaEventProxy::OnPlayerCommand(int32_t playerId, const char* message) {
	return core->eventHandler->OnPlayerCommand(playerId, message) ? 1 : 0;
}


uint8_t JavaEventProxy::OnPlayerPrivateMessage(int32_t playerId, int32_t targetPlayerId, const char* message) {
	return core->eventHandler->OnPlayerPrivateMessage(playerId, targetPlayerId, message) ? 1 : 0;
}


void JavaEventProxy::OnPlayerKeyBindDown(int32_t playerId, int32_t bindId) {
	core->eventHandler->OnPlayerKeyBindDown(playerId, bindId);
}


void JavaEventProxy::OnPlayerKeyBindUp(int32_t playerId, int32_t bindId) {
	core->eventHandler->OnPlayerKeyBindUp(playerId, bindId);
}


void JavaEventProxy::OnPlayerSpectate(int32_t playerId, int32_t targetPlayerId) {
	core->eventHandler->OnPlayerSpectate(playerId, targetPlayerId);
}


void JavaEventProxy::OnPlayerCrashReport(int32_t playerId, const char* report) {
	core->eventHandler->OnPlayerCrashReport(playerId, report);
}


void JavaEventProxy::OnVehicleUpdate(int32_t vehicleId, vcmpVehicleUpdate updateType) {
	core->eventHandler->OnVehicleUpdate(vehicleId, updateType);
}


void JavaEventProxy::OnVehicleExplode(int32_t vehicleId) {
	core->eventHandler->OnVehicleExplode(vehicleId);
}


void JavaEventProxy::OnVehicleRespawn(int32_t vehicleId) {
	core->eventHandler->OnVehicleRespawn(vehicleId);
}


void JavaEventProxy::OnObjectShot(int32_t objectId, int32_t playerId, int32_t weaponId) {
	core->eventHandler->OnObjectShot(objectId, playerId, weaponId);
}


void JavaEventProxy::OnObjectTouched(int32_t objectId, int32_t playerId) {
	core->eventHandler->OnObjectTouched(objectId, playerId);
}


uint8_t JavaEventProxy::OnPickupPickAttempt(int32_t pickupId, int32_t playerId) {
	return core->eventHandler->OnPickupPickAttempt(pickupId, playerId) ? 1 : 0;
}


void JavaEventProxy::OnPickupPicked(int32_t pickupId, int32_t playerId) {
	core->eventHandler->OnPickupPicked(pickupId, playerId);
}


void JavaEventProxy::OnPickupRespawn(int32_t pickupId) {
	core->eventHandler->OnPickupRespawn(pickupId);
}


void JavaEventProxy::OnCheckpointEntered(int32_t checkPointId, int32_t playerId) {
	core->eventHandler->OnCheckpointEntered(checkPointId, playerId);
}


void JavaEventProxy::OnCheckpointExited(int32_t checkPointId, int32_t playerId) {
	core->eventHandler->OnCheckpointExited(checkPointId, playerId);
}


void JavaEventProxy::OnEntityPoolChange(vcmpEntityPool entityType, int32_t entityId, uint8_t isDeleted) {
	core->eventHandler->OnEntityPoolChange(entityType, entityId, !!isDeleted);
}


void JavaEventProxy::OnServerPerformanceReport(size_t entryCount, const char** descriptions, uint64_t* times) {
	core->eventHandler->OnServerPerformanceReport(entryCount, descriptions, times);
}
