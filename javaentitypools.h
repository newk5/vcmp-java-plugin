#pragma once
#include "javaentitypool.h"

#define MAX_PLAYERS 100
#define MAX_VEHICLES 1000

class JavaConstructable {
public:
	JavaConstructable(void);
	~JavaConstructable(void);

	bool Setup(JNIEnv* jni, const char* className, const char* constructorSignature);
	jobject Create(JNIEnv* jni, ...);

	jclass klass;
	jmethodID constructor;
};

class JavaEntityPools {
public:
	JavaEntityPools(void);
	~JavaEntityPools(void);

	bool LoadBasicTypes(void);
	bool Initialise(bool isFirstBinding);
	void InitialisePoolStates(void);

	jobject CreateVector(JNIEnv* jni, float X, float Y, float Z);
	jobject CreateQuaternion(JNIEnv* jni, float X, float Y, float Z, float W);
	jobject CreateRotation2d(JNIEnv* jni, float horizontal, float vertical);
	jobject CreateVehicleColours(JNIEnv* jni, int primary, int secondary);
	jobject CreateCoordBlipInfo(JNIEnv* jni, int index, int worldId, float posX, float posY, float posZ, int scale, int colour, int spriteId);
	jobject CreateKeyBind(JNIEnv* jni, int id, bool onRelease, int keyOne, int keyTwo, int keyThree);
	jobject CreateWastedSettings(JNIEnv* jni, int deathTimeMillis, int fadeTimeMillis, float fadeInSpeed, float fadeOutSpeed, int fadeColour, int corpseFadeStart, int corpseFadeDuration);
	jobject CreateMapBounds(JNIEnv* jni, float maxX, float minX, float maxY, float minY);

	JavaEntityPool* players;
	JavaEntityPool* vehicles;
	JavaEntityPool* checkpoints;
	JavaEntityPool* objects;
	JavaEntityPool* pickups;

private:
	JavaConstructable vector;
	JavaConstructable quaternion;
	JavaConstructable rotation2d;
	JavaConstructable vehicleColours;
	JavaConstructable coordBlipInfo;
	JavaConstructable keyBind;
	JavaConstructable wastedSettings;
	JavaConstructable mapBounds;

};
