#include "javaentitypools.h"
#include "logging.h"
#include "main.h"

JavaConstructable::JavaConstructable(void) {
	klass = nullptr;
	constructor = nullptr;
}

JavaConstructable::~JavaConstructable(void) {
	if (klass != nullptr) {
		core->env->DeleteGlobalRef(klass);
	}
}

bool JavaConstructable::Setup(JNIEnv* jni, const char* className, const char* constructorSignature) {
	klass = (jclass)core->ReplaceWithGlobalRef(jni, jni->FindClass(className));
	if (klass == nullptr) {
		core->exc->PrintExceptions(__FUNCTION__);
		return Logging::Log(LogError, __FUNCTION__, "Could not find %s class.", className);
	}

	constructor = core->env->GetMethodID(klass, "<init>", constructorSignature);
	if (constructor == nullptr) {
		core->exc->PrintExceptions(__FUNCTION__);
		return Logging::Log(LogError, __FUNCTION__, "Could not find %s constructor from %s.", constructorSignature, className);
	}
	return true;
}

JavaEntityPools::JavaEntityPools(void) {
	players = new JavaEntityPool(MAX_PLAYERS, true);
	vehicles = new JavaEntityPool(MAX_VEHICLES, true);
	checkpoints = new JavaEntityPool(2000, false);
	objects = new JavaEntityPool(3000, false);
	pickups = new JavaEntityPool(2000, false);
}

JavaEntityPools::~JavaEntityPools(void) {
	delete players;
	delete vehicles;
	delete checkpoints;
	delete objects;
	delete pickups;
}

bool JavaEntityPools::LoadBasicTypes(void) {
	if (!vector.Setup(core->env, "com/maxorator/vcmp/java/plugin/integration/generic/Vector", "(FFF)V") ||
		!quaternion.Setup(core->env, "com/maxorator/vcmp/java/plugin/integration/generic/Quaternion", "(FFFF)V") ||
		!rotation2d.Setup(core->env, "com/maxorator/vcmp/java/plugin/integration/generic/Rotation2d", "(FF)V") ||
		!vehicleColours.Setup(core->env, "com/maxorator/vcmp/java/plugin/integration/vehicle/VehicleColours", "(II)V") ||
		!coordBlipInfo.Setup(core->env, "com/maxorator/vcmp/java/plugin/integration/server/CoordBlipInfo", "(IIFFFIII)V") ||
		!keyBind.Setup(core->env, "com/maxorator/vcmp/java/plugin/integration/server/KeyBind", "(IZIII)V") ||
		!wastedSettings.Setup(core->env, "com/maxorator/vcmp/java/plugin/integration/server/WastedSettings", "(IIFFIII)V") ||
		!mapBounds.Setup(core->env, "com/maxorator/vcmp/java/plugin/integration/server/MapBounds", "(FFFF)V")) {

		return false;
	}

	return true;
}

bool JavaEntityPools::Initialise(bool isFirstBinding) {
	if (!LoadBasicTypes()) {
		return false;
	} else if (!players->BindToClass("com/maxorator/vcmp/java/plugin/integration/player/PlayerImpl")) {
		return Logging::Log(LogError, __FUNCTION__, "Failed to load Player implementation class.");
	} else if (!vehicles->BindToClass("com/maxorator/vcmp/java/plugin/integration/vehicle/VehicleImpl")) {
		return Logging::Log(LogError, __FUNCTION__, "Failed to load Vehicle implementation class.");
	} else if (!checkpoints->BindToClass("com/maxorator/vcmp/java/plugin/integration/placeable/CheckPointImpl")) {
		return Logging::Log(LogError, __FUNCTION__, "Failed to load CheckPoint implementation class.");
	} else if (!objects->BindToClass("com/maxorator/vcmp/java/plugin/integration/placeable/GameObjectImpl")) {
		return Logging::Log(LogError, __FUNCTION__, "Failed to load GameObject implementation class.");
	} else if (!pickups->BindToClass("com/maxorator/vcmp/java/plugin/integration/placeable/PickupImpl")) {
		return Logging::Log(LogError, __FUNCTION__, "Failed to load Pickup implementation class.");
	}

	if (!isFirstBinding) {
		InitialisePoolStates();
	}

	return true;
}

void JavaEntityPools::InitialisePoolStates(void) {
	for (int i = 0; i < MAX_PLAYERS; i++) {
		if (api->IsPlayerConnected(i)) {
			players->Register(i);
		}
	}

	for (int i = 1; i < MAX_VEHICLES; i++) {
		if (api->CheckEntityExists(vcmpEntityPoolVehicle, i)) {
			vehicles->Register(i);
		}
	}
}

jobject JavaEntityPools::CreateVector(JNIEnv* jni, float X, float Y, float Z) {
	return jni->NewObject(vector.klass, vector.constructor, X, Y, Z);
}

jobject JavaEntityPools::CreateQuaternion(JNIEnv* jni, float X, float Y, float Z, float W) {
	return jni->NewObject(quaternion.klass, quaternion.constructor, X, Y, Z, W);
}

jobject JavaEntityPools::CreateRotation2d(JNIEnv* jni, float horizontal, float vertical) {
	return jni->NewObject(rotation2d.klass, rotation2d.constructor, horizontal, vertical);
}

jobject JavaEntityPools::CreateVehicleColours(JNIEnv* jni, int primary, int secondary) {
	return jni->NewObject(vehicleColours.klass, vehicleColours.constructor, primary, secondary);
}

jobject JavaEntityPools::CreateCoordBlipInfo(JNIEnv* jni, int index, int worldId, float posX, float posY, float posZ, int scale, int colour, int spriteId) {
	return jni->NewObject(coordBlipInfo.klass, coordBlipInfo.constructor, index, worldId, posX, posY, posZ, scale, colour, spriteId);
}

jobject JavaEntityPools::CreateKeyBind(JNIEnv* jni, int id, bool onRelease, int keyOne, int keyTwo, int keyThree) {
	return jni->NewObject(keyBind.klass, keyBind.constructor, id, onRelease, keyOne, keyTwo, keyThree);
}

jobject JavaEntityPools::CreateWastedSettings(JNIEnv* jni, int deathTimeMillis, int fadeTimeMillis, float fadeInSpeed, float fadeOutSpeed, int fadeColour, int corpseFadeStart, int corpseFadeDuration) {
	return jni->NewObject(wastedSettings.klass, wastedSettings.constructor, deathTimeMillis, fadeTimeMillis, fadeInSpeed, fadeOutSpeed, fadeColour, corpseFadeStart, corpseFadeDuration);
}

jobject JavaEntityPools::CreateMapBounds(JNIEnv* jni, float maxX, float minX, float maxY, float minY) {
	return jni->NewObject(mapBounds.klass, mapBounds.constructor, maxX, minX, maxY, minY);
}
