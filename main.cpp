#include "main.h"
#include "logging.h"
#include "javacore.h"
#include "javaeventproxy.h"
#include "platform.h"

#include <string.h>

PluginFuncs* api;
JavaCore* core;

extern "C" EXPORT unsigned int VcmpPluginInit(PluginFuncs* pluginFunctions, PluginCallbacks* pluginCallbacks, PluginInfo* pluginInfo) {
	Logging::Setup();
	Logging::Log(LogInfo, __FUNCTION__, "VC:MP Java integration plugin loading...");

	api = pluginFunctions;

	if (pluginInfo->structSize < sizeof(PluginInfo)) {
		Logging::Log(LogError, __FUNCTION__, "Incompatible (probaly older) server version.");
		return 0;
	}

	pluginInfo->pluginVersion = JAVA_PLUGIN_VERSION;
	Platform::strncpy(pluginInfo->name, "JavaIntegration", sizeof(pluginInfo->name));
	pluginInfo->apiMajorVersion = PLUGIN_API_MAJOR;
	pluginInfo->apiMinorVersion = PLUGIN_API_MINOR;

	if (!JavaEventProxy::StartVM(pluginCallbacks)) {
		return 0;
	}

	return 1;
}
