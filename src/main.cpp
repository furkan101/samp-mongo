#include <iostream>
#include <thread>
#include <vector>

#include "plugin.h"

#include <mongocxx/client.hpp>
#include <mongocxx/instance.hpp>
#include <mongocxx/uri.hpp>

extern void *pAMXFunctions;
void *pAMXFunctions;

static mongocxx::instance instance{};

PLUGIN_EXPORT unsigned int PLUGIN_CALL Supports() {
    return SUPPORTS_VERSION | SUPPORTS_AMX_NATIVES;
}

PLUGIN_EXPORT bool PLUGIN_CALL Load(void **ppData) {
    pAMXFunctions = ppData[PLUGIN_DATA_AMX_EXPORTS];
    
    printf(" [samp-mongo] MongoDB Driver initialized.\n");
    printf(" [samp-mongo] Plugin loaded successfully!\n");
    return true;
}

PLUGIN_EXPORT void PLUGIN_CALL Unload() {
    printf(" [samp-mongo] Plugin unloaded.\n");
}

PLUGIN_EXPORT int PLUGIN_CALL AmxLoad(AMX *amx) {
    return AMX_ERR_NONE;
}

PLUGIN_EXPORT int PLUGIN_CALL AmxUnload(AMX *amx) {
    return AMX_ERR_NONE;
}