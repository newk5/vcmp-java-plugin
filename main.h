#include <jni.h>
#include "plugin.h"
#include "javacore.h"

#ifdef _WIN32
#define EXPORT __declspec(dllexport)
#else
#define EXPORT
#endif

#define JAVA_PLUGIN_VERSION 0x200

extern PluginFuncs* api;
extern JavaCore* core;
