#include "main.h"
#include "javaeventhandler.h"
#include "javacore.h"
#include "logging.h"
#include "javastring.h"
#include "platform.h"
#include <string.h>

enum EventMethodName {
	FlagOnServerLoadScripts,
	FlagOnServerUnloadScripts,
	FlagOnServerInitialise,
	FlagOnServerShutdown,
	FlagOnServerFrame,
	FlagOnPluginCommand,
	FlagOnIncomingConnection,
	FlagOnClientScriptData,
	FlagOnPlayerConnect,
	FlagOnPlayerDisconnect,
	FlagOnPlayerRequestClass,
	FlagOnPlayerRequestSpawn,
	FlagOnPlayerSpawn,
	FlagOnPlayerDeath,
	FlagOnPlayerUpdate,
	FlagOnPlayerRequestEnterVehicle,
	FlagOnPlayerEnterVehicle,
	FlagOnPlayerExitVehicle,
	FlagOnPlayerNameChange,
	FlagOnPlayerStateChange,
	FlagOnPlayerActionChange,
	FlagOnPlayerOnFireChange,
	FlagOnPlayerCrouchChange,
	FlagOnPlayerGameKeysChange,
	FlagOnPlayerBeginTyping,
	FlagOnPlayerEndTyping,
	FlagOnPlayerAwayChange,
	FlagOnPlayerMessage,
	FlagOnPlayerCommand,
	FlagOnPlayerPrivateMessage,
	FlagOnPlayerKeyBindDown,
	FlagOnPlayerKeyBindUp,
	FlagOnPlayerSpectate,
	FlagOnPlayerCrashReport,
	FlagOnVehicleUpdate,
	FlagOnVehicleExplode,
	FlagOnVehicleRespawn,
	FlagOnObjectShot,
	FlagOnObjectTouched,
	FlagOnPickupPickAttempt,
	FlagOnPickupPicked,
	FlagOnPickupRespawn,
	FlagOnCheckPointEntered,
	FlagOnCheckPointExited
};

JavaEventHandler::JavaEventHandler(void) {
	memset(this, 0, sizeof(JavaEventHandler));
}

JavaEventHandler::~JavaEventHandler(void) {
	if (eventsInstance != nullptr) {
		core->env->DeleteGlobalRef(eventsInstance);
	}

	if (eventsClass != nullptr) {
		core->env->DeleteGlobalRef(eventsClass);
	}

	if (serverInstance != nullptr) {
		core->env->DeleteGlobalRef(serverInstance);
	}

	if (serverClass != nullptr) {
		core->env->DeleteGlobalRef(serverClass);
	}
}

bool JavaEventHandler::BindToClasses(bool isFirstBinding, int64_t flags, jobject existingEventsInstance) {
	if (existingEventsInstance == nullptr) {
		serverClass = (jclass)core->ReplaceWithGlobalRef(core->env, core->env->FindClass("com/maxorator/vcmp/java/plugin/integration/server/ServerImpl"));
		if (serverClass == nullptr) {
			core->env->ExceptionClear();
			return Logging::Log(LogError, __FUNCTION__, "Could not find server class.");
		}

		jmethodID serverConstructor = core->env->GetMethodID(serverClass, "<init>", "()V");
		if (serverConstructor == nullptr) {
			core->env->ExceptionClear();
			return Logging::Log(LogError, __FUNCTION__, "Could not find server class constructor.");
		}

		serverInstance = core->ReplaceWithGlobalRef(core->env, core->env->NewObject(serverClass, serverConstructor));
		if (serverInstance == nullptr) {
			core->env->ExceptionClear();
			return Logging::Log(LogError, __FUNCTION__, "Could not create instance of server class.");
		}

		eventsClass = (jclass)core->ReplaceWithGlobalRef(core->env, core->env->FindClass(core->GetMainClass()));
		if (eventsClass == nullptr) {
			core->env->ExceptionClear();
			return Logging::Log(LogError, __FUNCTION__, "Could not find events class.");
		}

		jmethodID eventsConstructor = core->env->GetMethodID(eventsClass, "<init>", "(Lcom/maxorator/vcmp/java/plugin/integration/server/Server;)V");
		if (eventsConstructor == nullptr) {
			core->env->ExceptionClear();
			return Logging::Log(LogError, __FUNCTION__, "Could not find events class constructor.");
		}

		eventsInstance = core->ReplaceWithGlobalRef(core->env, core->env->NewObject(eventsClass, eventsConstructor, serverInstance));
		if (eventsInstance == nullptr) {
			core->env->ExceptionClear();
			return Logging::Log(LogError, __FUNCTION__, "Could not create instance of events class.");
		}
	}
	else {
		eventsInstance = core->env->NewGlobalRef(existingEventsInstance);
		if (eventsInstance == nullptr) {
			core->env->ExceptionClear();
			return Logging::Log(LogError, __FUNCTION__, "Could not store instance of events class.");
		}

		eventsClass = (jclass)core->ReplaceWithGlobalRef(core->env, core->env->GetObjectClass(eventsInstance));
		if (eventsClass == nullptr) {
			core->env->ExceptionClear();
			return Logging::Log(LogError, __FUNCTION__, "Could not get events class.");
		}
	}

	onLoadScriptsMethod = GetOwnMethod(eventsClass, flags & (1LL << FlagOnServerLoadScripts), "onServerLoadScripts", "()V");
	unUnloadScriptsMethod = core->env->GetMethodID(eventsClass, "onServerUnloadScriptsInternal", "()V");

	onServerInitialiseMethod = GetOwnMethod(eventsClass, flags & (1LL << FlagOnServerInitialise), "onServerInitialise", "()Z");
	onServerShutdownMethod = GetOwnMethod(eventsClass, flags & (1LL << FlagOnServerShutdown), "onServerShutdown", "()V");
	onServerFrameMethod = core->env->GetMethodID(eventsClass, "onServerFrameInternal", "()V");

	onPluginCommandMethod = GetOwnMethod(eventsClass, flags & (1LL << FlagOnPluginCommand), "onPluginCommand", "(ILjava/lang/String;)V");
	onIncomingConnectionMethod = GetOwnMethod(eventsClass, flags & (1LL << FlagOnIncomingConnection), "onIncomingConnection", "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)Ljava/lang/String;");
	onClientScriptDataMethod = GetOwnMethod(eventsClass, flags & (1LL << FlagOnClientScriptData), "onClientScriptData", "(Lcom/maxorator/vcmp/java/plugin/integration/player/Player;[B)V");

	onPlayerConnectMethod = GetOwnMethod(eventsClass, flags & (1LL << FlagOnPlayerConnect), "onPlayerConnect", "(Lcom/maxorator/vcmp/java/plugin/integration/player/Player;)V");
	onPlayerDisconnectMethod = GetOwnMethod(eventsClass, flags & (1LL << FlagOnPlayerDisconnect), "onPlayerDisconnect", "(Lcom/maxorator/vcmp/java/plugin/integration/player/Player;I)V");
	
	onPlayerRequestClassMethod = GetOwnMethod(eventsClass, flags & (1LL << FlagOnPlayerRequestClass), "onPlayerRequestClass", "(Lcom/maxorator/vcmp/java/plugin/integration/player/Player;I)Z");
	onPlayerRequestSpawnMethod = GetOwnMethod(eventsClass, flags & (1LL << FlagOnPlayerRequestSpawn), "onPlayerRequestSpawn", "(Lcom/maxorator/vcmp/java/plugin/integration/player/Player;)Z");
	onPlayerSpawnMethod = GetOwnMethod(eventsClass, flags & (1LL << FlagOnPlayerSpawn), "onPlayerSpawn", "(Lcom/maxorator/vcmp/java/plugin/integration/player/Player;)V");
	onPlayerDeathMethod = GetOwnMethod(eventsClass, flags & (1LL << FlagOnPlayerDeath), "onPlayerDeath", "(Lcom/maxorator/vcmp/java/plugin/integration/player/Player;Lcom/maxorator/vcmp/java/plugin/integration/player/Player;II)V");
	onPlayerUpdateMethod = GetOwnMethod(eventsClass, flags & (1LL << FlagOnPlayerUpdate), "onPlayerUpdate", "(Lcom/maxorator/vcmp/java/plugin/integration/player/Player;I)V");
	
	onPlayerRequestEnterVehicleMethod = GetOwnMethod(eventsClass, flags & (1LL << FlagOnPlayerRequestEnterVehicle), "onPlayerRequestEnterVehicle", "(Lcom/maxorator/vcmp/java/plugin/integration/player/Player;Lcom/maxorator/vcmp/java/plugin/integration/vehicle/Vehicle;I)Z");
	onPlayerEnterVehicleMethod = GetOwnMethod(eventsClass, flags & (1LL << FlagOnPlayerEnterVehicle), "onPlayerEnterVehicle", "(Lcom/maxorator/vcmp/java/plugin/integration/player/Player;Lcom/maxorator/vcmp/java/plugin/integration/vehicle/Vehicle;I)V");
	onPlayerExitVehicleMethod = GetOwnMethod(eventsClass, flags & (1LL << FlagOnPlayerExitVehicle), "onPlayerExitVehicle", "(Lcom/maxorator/vcmp/java/plugin/integration/player/Player;Lcom/maxorator/vcmp/java/plugin/integration/vehicle/Vehicle;)V");
	
	onPlayerNameChangeMethod = GetOwnMethod(eventsClass, flags & (1LL << FlagOnPlayerNameChange), "onPlayerNameChange", "(Lcom/maxorator/vcmp/java/plugin/integration/player/Player;Ljava/lang/String;Ljava/lang/String;)V");
	onPlayerStateChangeMethod = GetOwnMethod(eventsClass, flags & (1LL << FlagOnPlayerStateChange), "onPlayerStateChange", "(Lcom/maxorator/vcmp/java/plugin/integration/player/Player;II)V");
	onPlayerActionChangeMethod = GetOwnMethod(eventsClass, flags & (1LL << FlagOnPlayerActionChange), "onPlayerActionChange", "(Lcom/maxorator/vcmp/java/plugin/integration/player/Player;II)V");
	onPlayerOnFireChangeMethod = GetOwnMethod(eventsClass, flags & (1LL << FlagOnPlayerOnFireChange), "onPlayerOnFireChange", "(Lcom/maxorator/vcmp/java/plugin/integration/player/Player;Z)V");
	onPlayerCrouchChangeMethod = GetOwnMethod(eventsClass, flags & (1LL << FlagOnPlayerCrouchChange), "onPlayerCrouchChange", "(Lcom/maxorator/vcmp/java/plugin/integration/player/Player;Z)V");
	onPlayerGameKeysChangeMethod = GetOwnMethod(eventsClass, flags & (1LL << FlagOnPlayerGameKeysChange), "onPlayerGameKeysChange", "(Lcom/maxorator/vcmp/java/plugin/integration/player/Player;II)V");
	onPlayerBeginTypingMethod = GetOwnMethod(eventsClass, flags & (1LL << FlagOnPlayerBeginTyping), "onPlayerBeginTyping", "(Lcom/maxorator/vcmp/java/plugin/integration/player/Player;)V");
	onPlayerEndTypingMethod = GetOwnMethod(eventsClass, flags & (1LL << FlagOnPlayerEndTyping), "onPlayerEndTyping", "(Lcom/maxorator/vcmp/java/plugin/integration/player/Player;)V");
	onPlayerAwayChangeMethod = GetOwnMethod(eventsClass, flags & (1LL << FlagOnPlayerAwayChange), "onPlayerAwayChange", "(Lcom/maxorator/vcmp/java/plugin/integration/player/Player;Z)V");
	
	onPlayerMessageMethod = GetOwnMethod(eventsClass, flags & (1LL << FlagOnPlayerMessage), "onPlayerMessage", "(Lcom/maxorator/vcmp/java/plugin/integration/player/Player;Ljava/lang/String;)Z");
	onPlayerCommandMethod = GetOwnMethod(eventsClass, flags & (1LL << FlagOnPlayerCommand), "onPlayerCommand", "(Lcom/maxorator/vcmp/java/plugin/integration/player/Player;Ljava/lang/String;)Z");
	onPlayerPrivateMessageMethod = GetOwnMethod(eventsClass, flags & (1LL << FlagOnPlayerPrivateMessage), "onPlayerPrivateMessage", "(Lcom/maxorator/vcmp/java/plugin/integration/player/Player;Lcom/maxorator/vcmp/java/plugin/integration/player/Player;Ljava/lang/String;)Z");
	
	onPlayerKeyBindDownMethod = GetOwnMethod(eventsClass, flags & (1LL << FlagOnPlayerKeyBindDown), "onPlayerKeyBindDown", "(Lcom/maxorator/vcmp/java/plugin/integration/player/Player;I)V");
	onPlayerKeyBindUpMethod = GetOwnMethod(eventsClass, flags & (1LL << FlagOnPlayerKeyBindUp), "onPlayerKeyBindUp", "(Lcom/maxorator/vcmp/java/plugin/integration/player/Player;I)V");
	onPlayerSpectateMethod = GetOwnMethod(eventsClass, flags & (1LL << FlagOnPlayerSpectate), "onPlayerSpectate", "(Lcom/maxorator/vcmp/java/plugin/integration/player/Player;Lcom/maxorator/vcmp/java/plugin/integration/player/Player;)V");

	onPlayerCrashReportMethod = GetOwnMethod(eventsClass, flags & (1LL << FlagOnPlayerCrashReport), "onPlayerCrashReport", "(Lcom/maxorator/vcmp/java/plugin/integration/player/Player;Ljava/lang/String;)V");

	onVehicleUpdateMethod = GetOwnMethod(eventsClass, flags & (1LL << FlagOnVehicleUpdate), "onVehicleUpdate", "(Lcom/maxorator/vcmp/java/plugin/integration/vehicle/Vehicle;I)V");
	onVehicleExplodeMethod = GetOwnMethod(eventsClass, flags & (1LL << FlagOnVehicleExplode), "onVehicleExplode", "(Lcom/maxorator/vcmp/java/plugin/integration/vehicle/Vehicle;)V");
	onVehicleRespawnMethod = GetOwnMethod(eventsClass, flags & (1LL << FlagOnVehicleRespawn), "onVehicleRespawn", "(Lcom/maxorator/vcmp/java/plugin/integration/vehicle/Vehicle;)V");

	onObjectShotMethod = GetOwnMethod(eventsClass, flags & (1LL << FlagOnObjectShot), "onObjectShot", "(Lcom/maxorator/vcmp/java/plugin/integration/placeable/GameObject;Lcom/maxorator/vcmp/java/plugin/integration/player/Player;I)V");
	onObjectTouchedMethod = GetOwnMethod(eventsClass, flags & (1LL << FlagOnObjectTouched), "onObjectTouched", "(Lcom/maxorator/vcmp/java/plugin/integration/placeable/GameObject;Lcom/maxorator/vcmp/java/plugin/integration/player/Player;)V");

	onPickupPickAttemptMethod = GetOwnMethod(eventsClass, flags & (1LL << FlagOnPickupPickAttempt), "onPickupPickAttempt", "(Lcom/maxorator/vcmp/java/plugin/integration/placeable/Pickup;Lcom/maxorator/vcmp/java/plugin/integration/player/Player;)Z");
	onPickupPickedMethod = GetOwnMethod(eventsClass, flags & (1LL << FlagOnPickupPicked), "onPickupPicked", "(Lcom/maxorator/vcmp/java/plugin/integration/placeable/Pickup;Lcom/maxorator/vcmp/java/plugin/integration/player/Player;)V");
	onPickupRespawnMethod = GetOwnMethod(eventsClass, flags & (1LL << FlagOnPickupRespawn), "onPickupRespawn", "(Lcom/maxorator/vcmp/java/plugin/integration/placeable/Pickup;)V");

	onCheckpointEnteredMethod = GetOwnMethod(eventsClass, flags & (1LL << FlagOnCheckPointEntered), "onCheckPointEntered", "(Lcom/maxorator/vcmp/java/plugin/integration/placeable/CheckPoint;Lcom/maxorator/vcmp/java/plugin/integration/player/Player;)V");
	onCheckpointExitedMethod = GetOwnMethod(eventsClass, flags & (1LL << FlagOnCheckPointExited), "onCheckPointExited", "(Lcom/maxorator/vcmp/java/plugin/integration/placeable/CheckPoint;Lcom/maxorator/vcmp/java/plugin/integration/player/Player;)V");

	core->env->ExceptionClear();

	return true;
}

jmethodID JavaEventHandler::GetOwnMethod(jclass klass, int64_t flagMatch, const char* name, const char* signature) {
	if (flagMatch == 0) {
		return nullptr;
	}

	return FilterOwnMethod(core->env->GetMethodID(klass, name, signature), klass);
}

jmethodID JavaEventHandler::FilterOwnMethod(jmethodID methodId, jclass match) {
	jclass declaringClass;

	if (methodId != nullptr) {
		if (core->jvmti->GetMethodDeclaringClass(methodId, &declaringClass) == JVMTI_ERROR_NONE) {
			if (core->env->IsSameObject(declaringClass, match)) {
				return methodId;
			}
		}
	}

	return nullptr;
}

void JavaEventHandler::OnLoadScripts(void) {
	if (onLoadScriptsMethod) {
		core->env->CallVoidMethod(eventsInstance, onLoadScriptsMethod);
		core->exc->PrintExceptions(__FUNCTION__);
	}
}

void JavaEventHandler::OnUnloadScripts(void) {
	if (unUnloadScriptsMethod) {
		core->env->CallVoidMethod(eventsInstance, unUnloadScriptsMethod);
		core->exc->PrintExceptions(__FUNCTION__);
	}
}

bool JavaEventHandler::OnServerInitialise(void) {
	Logging::Log(LogInfo, __FUNCTION__, "Server initialised, scripts loaded.");

	if (onServerInitialiseMethod) {
		jboolean result = core->env->CallBooleanMethod(eventsInstance, onServerInitialiseMethod);
		core->exc->PrintExceptions(__FUNCTION__);
		OnLoadScripts();
		return !!result;
	}

	OnLoadScripts();

	return true;
}

void JavaEventHandler::OnServerShutdown(void) {
	OnUnloadScripts();

	if (onServerShutdownMethod) {
		core->env->CallVoidMethod(eventsInstance, onServerShutdownMethod);
		core->exc->PrintExceptions(__FUNCTION__);
	}
}

void JavaEventHandler::OnServerFrame(float elapsedTime) {
	if (onServerFrameMethod) {
		core->env->CallVoidMethod(eventsInstance, onServerFrameMethod);
		core->exc->PrintExceptions(__FUNCTION__);
	}
}

bool JavaEventHandler::OnPluginCommand(uint32_t commandIdentifier, const char* message) {
	if (onPluginCommandMethod) {
		jstring messageString = core->env->NewStringUTF(message);

		core->env->CallVoidMethod(eventsInstance, onPluginCommandMethod, commandIdentifier, messageString);
		core->exc->PrintExceptions(__FUNCTION__);

		core->env->DeleteLocalRef(messageString);
	}

	return true;
}

bool JavaEventHandler::OnIncomingConnection(char* playerName, size_t nameBufferSize, const char* userPassword, const char* ipAddress) {
	bool returnValue = true;

	if (onIncomingConnectionMethod) {
		core->env->PushLocalFrame(10);

		jobject result = core->env->CallObjectMethod(eventsInstance, onIncomingConnectionMethod, core->env->NewStringUTF(playerName), core->env->NewStringUTF(userPassword), core->env->NewStringUTF(ipAddress));
		core->exc->PrintExceptions(__FUNCTION__);

		if (result != nullptr) {
			JavaString newName(core->env, (jstring)result);

			if (strcmp(newName.text, playerName) != 0) {
				Platform::strncpy(playerName, newName.text, 24);
			}
		}
		else {
			returnValue = false;
		}

		core->env->PopLocalFrame(nullptr);
	}

	return returnValue;
}

void JavaEventHandler::OnClientScriptData(int32_t playerId, const uint8_t* data, size_t size) {
	if (onClientScriptDataMethod != nullptr) {
		jbyteArray dataArray = core->env->NewByteArray((jsize)size);

		if (dataArray == nullptr) {
			core->exc->PrintExceptions(__FUNCTION__);
		}
		else {
			core->env->SetByteArrayRegion(dataArray, 0, (jsize)size, (const jbyte*)data);

			if (!core->exc->PrintExceptions(__FUNCTION__)) {
				core->env->CallVoidMethod(eventsInstance, onClientScriptDataMethod, core->pools->players->Get(core->env, playerId), dataArray);
			}

			core->env->DeleteLocalRef(dataArray);
		}
	}
}

void JavaEventHandler::OnPlayerConnect(int32_t playerId) {
	core->pools->players->Register(playerId);

	if (onPlayerConnectMethod) {
		core->env->CallVoidMethod(eventsInstance, onPlayerConnectMethod, core->pools->players->Get(core->env, playerId));
		core->exc->PrintExceptions(__FUNCTION__);
	}
}

void JavaEventHandler::OnPlayerDisconnect(int32_t playerId, vcmpDisconnectReason reason) {
	if (onPlayerDisconnectMethod) {
		core->env->CallVoidMethod(eventsInstance, onPlayerDisconnectMethod, core->pools->players->Get(core->env, playerId), reason);
		core->exc->PrintExceptions(__FUNCTION__);
	}

	core->pools->players->Delete(playerId);
}

bool JavaEventHandler::OnPlayerRequestClass(int32_t playerId, int32_t offset) {
	if (onPlayerRequestClassMethod) {
		jboolean result = core->env->CallBooleanMethod(eventsInstance, onPlayerRequestClassMethod, core->pools->players->Get(core->env, playerId), offset);
		core->exc->PrintExceptions(__FUNCTION__);
		return !!result;
	}

	return true;
}

bool JavaEventHandler::OnPlayerRequestSpawn(int32_t playerId) {
	if (onPlayerRequestSpawnMethod) {
		jboolean result = core->env->CallBooleanMethod(eventsInstance, onPlayerRequestSpawnMethod, core->pools->players->Get(core->env, playerId));
		core->exc->PrintExceptions(__FUNCTION__);
		return !!result;
	}

	return true;
}

void JavaEventHandler::OnPlayerSpawn(int32_t playerId) {
	if (onPlayerSpawnMethod) {
		core->env->CallVoidMethod(eventsInstance, onPlayerSpawnMethod, core->pools->players->Get(core->env, playerId));
		core->exc->PrintExceptions(__FUNCTION__);
	}
}

void JavaEventHandler::OnPlayerDeath(int32_t playerId, int32_t killerId, int32_t reason, vcmpBodyPart bodyPart) {
	if (onPlayerDeathMethod) {
		core->env->CallVoidMethod(eventsInstance, onPlayerDeathMethod, core->pools->players->Get(core->env, playerId), killerId == -1 ? nullptr : core->pools->players->Get(core->env, killerId), reason, bodyPart);
		core->exc->PrintExceptions(__FUNCTION__);
	}
}

void JavaEventHandler::OnPlayerUpdate(int32_t playerId, vcmpPlayerUpdate updateType) {
	if (onPlayerUpdateMethod) {
		core->env->CallVoidMethod(eventsInstance, onPlayerUpdateMethod, core->pools->players->Get(core->env, playerId), updateType);
		core->exc->PrintExceptions(__FUNCTION__);
	}
}

bool JavaEventHandler::OnPlayerRequestEnterVehicle(int32_t playerId, int32_t vehicleId, int32_t slotIndex) {
	if (onPlayerRequestEnterVehicleMethod) {
		jboolean result = core->env->CallBooleanMethod(eventsInstance, onPlayerRequestEnterVehicleMethod, core->pools->players->Get(core->env, playerId), core->pools->vehicles->Get(core->env, vehicleId), slotIndex);
		core->exc->PrintExceptions(__FUNCTION__);
		return !!result;
	}

	return true;
}

void JavaEventHandler::OnPlayerEnterVehicle(int32_t playerId, int32_t vehicleId, int32_t slotIndex) {
	if (onPlayerEnterVehicleMethod) {
		core->env->CallVoidMethod(eventsInstance, onPlayerEnterVehicleMethod, core->pools->players->Get(core->env, playerId), core->pools->vehicles->Get(core->env, vehicleId), slotIndex);
		core->exc->PrintExceptions(__FUNCTION__);
	}
}

void JavaEventHandler::OnPlayerExitVehicle(int32_t playerId, int32_t vehicleId) {
	if (onPlayerExitVehicleMethod) {
		core->env->CallVoidMethod(eventsInstance, onPlayerExitVehicleMethod, core->pools->players->Get(core->env, playerId), core->pools->vehicles->Get(core->env, vehicleId));
		core->exc->PrintExceptions(__FUNCTION__);
	}
}

void JavaEventHandler::OnPlayerNameChange(int32_t playerId, const char* oldName, const char* newName) {
	if (onPlayerNameChangeMethod) {
		jstring oldNameString = core->env->NewStringUTF(oldName);
		jstring newNameString = core->env->NewStringUTF(newName);

		core->env->CallVoidMethod(eventsInstance, onPlayerNameChangeMethod, core->pools->players->Get(core->env, playerId), oldNameString, newNameString);
		core->exc->PrintExceptions(__FUNCTION__);

		core->env->DeleteLocalRef(oldNameString);
		core->env->DeleteLocalRef(newNameString);
	}
}

void JavaEventHandler::OnPlayerStateChange(int32_t playerId, vcmpPlayerState oldState, vcmpPlayerState newState) {
	if (onPlayerStateChangeMethod) {
		core->env->CallVoidMethod(eventsInstance, onPlayerStateChangeMethod, core->pools->players->Get(core->env, playerId), oldState, newState);
		core->exc->PrintExceptions(__FUNCTION__);
	}
}

void JavaEventHandler::OnPlayerActionChange(int32_t playerId, int32_t oldAction, int32_t newAction) {
	if (onPlayerActionChangeMethod) {
		core->env->CallVoidMethod(eventsInstance, onPlayerActionChangeMethod, core->pools->players->Get(core->env, playerId), oldAction, newAction);
		core->exc->PrintExceptions(__FUNCTION__);
	}
}

void JavaEventHandler::OnPlayerOnFireChange(int32_t playerId, bool isOnFire) {
	if (onPlayerOnFireChangeMethod) {
		core->env->CallVoidMethod(eventsInstance, onPlayerOnFireChangeMethod, core->pools->players->Get(core->env, playerId), isOnFire);
		core->exc->PrintExceptions(__FUNCTION__);
	}
}

void JavaEventHandler::OnPlayerCrouchChange(int32_t playerId, bool isCrouching) {
	if (onPlayerCrouchChangeMethod) {
		core->env->CallVoidMethod(eventsInstance, onPlayerCrouchChangeMethod, core->pools->players->Get(core->env, playerId), (jboolean)isCrouching);
		core->exc->PrintExceptions(__FUNCTION__);
	}
}

void JavaEventHandler::OnPlayerGameKeysChange(int32_t playerId, uint32_t oldKeys, uint32_t newKeys) {
	if (onPlayerGameKeysChangeMethod) {
		core->env->CallVoidMethod(eventsInstance, onPlayerGameKeysChangeMethod, core->pools->players->Get(core->env, playerId), oldKeys, newKeys);
		core->exc->PrintExceptions(__FUNCTION__);
	}
}

void JavaEventHandler::OnPlayerBeginTyping(int32_t playerId) {
	if (onPlayerBeginTypingMethod) {
		core->env->CallVoidMethod(eventsInstance, onPlayerBeginTypingMethod, core->pools->players->Get(core->env, playerId));
		core->exc->PrintExceptions(__FUNCTION__);
	}
}

void JavaEventHandler::OnPlayerEndTyping(int32_t playerId) {
	if (onPlayerEndTypingMethod) {
		core->env->CallVoidMethod(eventsInstance, onPlayerEndTypingMethod, core->pools->players->Get(core->env, playerId));
		core->exc->PrintExceptions(__FUNCTION__);
	}
}

void JavaEventHandler::OnPlayerAwayChange(int32_t playerId, bool isAway) {
	if (onPlayerAwayChangeMethod) {
		core->env->CallVoidMethod(eventsInstance, onPlayerAwayChangeMethod, core->pools->players->Get(core->env, playerId), isAway);
		core->exc->PrintExceptions(__FUNCTION__);
	}
}

bool JavaEventHandler::OnPlayerMessage(int32_t nPlayerId, const char* text) {
	if (onPlayerMessageMethod) {
		jstring textString = core->env->NewStringUTF(text);

		jboolean result = core->env->CallBooleanMethod(eventsInstance, onPlayerMessageMethod, core->pools->players->Get(core->env, nPlayerId), textString);
		core->exc->PrintExceptions(__FUNCTION__);

		core->env->DeleteLocalRef(textString);
		return !!result;
	}

	return true;
}

bool JavaEventHandler::OnPlayerCommand(int32_t playerId, const char* text) {
	if (onPlayerCommandMethod) {
		jstring textString = core->env->NewStringUTF(text);

		jboolean result = core->env->CallBooleanMethod(eventsInstance, onPlayerCommandMethod, core->pools->players->Get(core->env, playerId), textString);
		core->exc->PrintExceptions(__FUNCTION__);

		core->env->DeleteLocalRef(textString);
		return !!result;
	}

	return false;
}

bool JavaEventHandler::OnPlayerPrivateMessage(int32_t playerId, int32_t targetPlayerId, const char* text) {
	if (onPlayerPrivateMessageMethod) {
		jstring textString = core->env->NewStringUTF(text);

		jboolean result = core->env->CallBooleanMethod(eventsInstance, onPlayerPrivateMessageMethod, core->pools->players->Get(core->env, playerId), core->pools->players->Get(core->env, targetPlayerId), textString);
		core->exc->PrintExceptions(__FUNCTION__);

		core->env->DeleteLocalRef(textString);
		return !!result;
	}

	return true;
}

void JavaEventHandler::OnPlayerKeyBindDown(int32_t playerId, int32_t bindId) {
	if (onPlayerKeyBindDownMethod) {
		core->env->CallVoidMethod(eventsInstance, onPlayerKeyBindDownMethod, core->pools->players->Get(core->env, playerId), bindId);
		core->exc->PrintExceptions(__FUNCTION__);
	}
}

void JavaEventHandler::OnPlayerKeyBindUp(int32_t playerId, int32_t bindId) {
	if (onPlayerKeyBindUpMethod) {
		core->env->CallVoidMethod(eventsInstance, onPlayerKeyBindUpMethod, core->pools->players->Get(core->env, playerId), bindId);
		core->exc->PrintExceptions(__FUNCTION__);
	}
}

void JavaEventHandler::OnPlayerSpectate(int32_t playerId, int32_t targetId) {
	if (onPlayerSpectateMethod) {
		core->env->CallVoidMethod(eventsInstance, onPlayerSpectateMethod, core->pools->checkpoints->Get(core->env, playerId), targetId == -1 ? nullptr : core->pools->players->Get(core->env, targetId));
		core->exc->PrintExceptions(__FUNCTION__);
	}
}

void JavaEventHandler::OnPlayerCrashReport(int32_t playerId, const char* reportText) {

	if (onPlayerCrashReportMethod) {
		jstring textString = core->env->NewStringUTF(reportText);

		core->env->CallVoidMethod(eventsInstance, onPlayerCrashReportMethod, core->pools->players->Get(core->env, playerId), textString);
		core->exc->PrintExceptions(__FUNCTION__);
		core->env->DeleteLocalRef(textString);
	}

}

void JavaEventHandler::OnVehicleUpdate(int32_t vehicleId, vcmpVehicleUpdate updateType) {
	if (onVehicleUpdateMethod) {
		core->env->CallVoidMethod(eventsInstance, onVehicleUpdateMethod, core->pools->vehicles->Get(core->env, vehicleId), updateType);
		core->exc->PrintExceptions(__FUNCTION__);
	}
}

void JavaEventHandler::OnVehicleExplode(int32_t vehicleId) {
	if (onVehicleExplodeMethod) {
		core->env->CallVoidMethod(eventsInstance, onVehicleExplodeMethod, core->pools->vehicles->Get(core->env, vehicleId));
		core->exc->PrintExceptions(__FUNCTION__);
	}
}

void JavaEventHandler::OnVehicleRespawn(int32_t vehicleId) {
	if (onVehicleRespawnMethod) {
		core->env->CallVoidMethod(eventsInstance, onVehicleRespawnMethod, core->pools->vehicles->Get(core->env, vehicleId));
		core->exc->PrintExceptions(__FUNCTION__);
	}
}

void JavaEventHandler::OnObjectShot(int32_t objectId, int32_t playerId, int32_t weaponId) {
	if (onObjectShotMethod) {
		core->env->CallVoidMethod(eventsInstance, onObjectShotMethod, core->pools->objects->Get(core->env, objectId), core->pools->players->Get(core->env, playerId), weaponId);
		core->exc->PrintExceptions(__FUNCTION__);
	}
}

void JavaEventHandler::OnObjectTouched(int32_t objectId, int32_t playerId) {
	if (onObjectTouchedMethod) {
		core->env->CallVoidMethod(eventsInstance, onObjectTouchedMethod, core->pools->objects->Get(core->env, objectId), core->pools->players->Get(core->env, playerId));
		core->exc->PrintExceptions(__FUNCTION__);
	}
}

bool JavaEventHandler::OnPickupPickAttempt(int32_t pickupId, int32_t playerId) {
	if (onPickupPickAttemptMethod) {
		jboolean result = core->env->CallBooleanMethod(eventsInstance, onPickupPickAttemptMethod, core->pools->pickups->Get(core->env, pickupId), core->pools->players->Get(core->env, playerId));
		core->exc->PrintExceptions(__FUNCTION__);
		return !!result;
	}

	return true;
}

void JavaEventHandler::OnPickupPicked(int32_t pickupId, int32_t playerId) {
	if (onPickupPickedMethod) {
		core->env->CallVoidMethod(eventsInstance, onPickupPickedMethod, core->pools->pickups->Get(core->env, pickupId), core->pools->players->Get(core->env, playerId));
		core->exc->PrintExceptions(__FUNCTION__);
	}
}

void JavaEventHandler::OnPickupRespawn(int32_t pickupId) {
	if (onPickupRespawnMethod) {
		core->env->CallVoidMethod(eventsInstance, onPickupRespawnMethod, core->pools->pickups->Get(core->env, pickupId));
		core->exc->PrintExceptions(__FUNCTION__);
	}
}

void JavaEventHandler::OnCheckpointEntered(int32_t checkpointId, int32_t playerId) {
	if (onCheckpointEnteredMethod) {
		core->env->CallVoidMethod(eventsInstance, onCheckpointEnteredMethod, core->pools->checkpoints->Get(core->env, checkpointId), core->pools->players->Get(core->env, playerId));
		core->exc->PrintExceptions(__FUNCTION__);
	}
}

void JavaEventHandler::OnCheckpointExited(int32_t checkpointId, int32_t playerId) {
	if (onCheckpointExitedMethod) {
		core->env->CallVoidMethod(eventsInstance, onCheckpointExitedMethod, core->pools->checkpoints->Get(core->env, checkpointId), core->pools->players->Get(core->env, playerId));
		core->exc->PrintExceptions(__FUNCTION__);
	}
}

void JavaEventHandler::OnEntityPoolChange(vcmpEntityPool entityType, int32_t entityId, bool isDeleted) {
	if (isDeleted) {
		switch (entityType) {
			case vcmpEntityPoolVehicle:
				core->pools->vehicles->Delete(entityId);
				break;
			case vcmpEntityPoolObject:
				core->pools->objects->Delete(entityId);
				break;
			case vcmpEntityPoolPickup:
				core->pools->objects->Delete(entityId);
				break;
			case vcmpEntityPoolCheckPoint:
				core->pools->checkpoints->Delete(entityId);
				break;
		}
	}
	else {
		switch (entityType) {
			case vcmpEntityPoolVehicle:
				core->pools->vehicles->Register(entityId);
				break;
			case vcmpEntityPoolObject:
				core->pools->objects->Register(entityId);
				break;
			case vcmpEntityPoolPickup:
				core->pools->objects->Register(entityId);
				break;
			case vcmpEntityPoolCheckPoint:
				core->pools->checkpoints->Register(entityId);
				break;
		}
	}
}

void JavaEventHandler::OnServerPerformanceReport(size_t entryCount, const char** descriptions, uint64_t* times) {

}
