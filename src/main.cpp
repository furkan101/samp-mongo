#include <amx/amx.h>
#include <plugincommon.h>
#include "worker.h"
#include "natives.h"

typedef void (*logprintf_t)(char* format, ...);
logprintf_t logprintf;
void *pAMXFunctions;

int ExecCallback(AMX* amx, const char* func, int playerid, int resultIdx) {
    int idx;
    if (amx_FindPublic(amx, func, &idx) == AMX_ERR_NONE) {
        amx_Push(amx, playerid);
        amx_Push(amx, resultIdx);
        cell retval;
        amx_Exec(amx, &retval, idx);
        return 1;
    }
    return 0;
}

void Log(const char* fmt, ...) {
    logprintf((char*)fmt); 
}

extern "C" {
    
    unsigned int PLUGIN_CALL Supports() {
        return SUPPORTS_VERSION | SUPPORTS_AMX_NATIVES | SUPPORTS_PROCESS_TICK;
    }

    bool PLUGIN_CALL Load(void **ppData) {
        pAMXFunctions = ppData[PLUGIN_DATA_AMX_EXPORTS];
        logprintf = (logprintf_t)ppData[PLUGIN_DATA_LOGPRINTF];
        
        StartWorker();
        logprintf(" >> samp-mongo v2: MODULAR SYSTEM LOADED!");
        return true;
    }

    void PLUGIN_CALL Unload() {
        StopWorker();
        logprintf(" >> samp-mongo v2: Unloaded");
    }

    int PLUGIN_CALL AmxLoad(AMX* amx) {
        return amx_Register(amx, PluginNatives, -1);
    }

    int PLUGIN_CALL AmxUnload(AMX* amx) {
        return AMX_ERR_NONE;
    }

    void PLUGIN_CALL ProcessTick() {
        ProcessCallbacks();
    }
}