#include <amx/amx.h>
#include <plugincommon.h>

typedef void (*logprintf_t)(char* format, ...);
logprintf_t logprintf;

void *pAMXFunctions;

extern "C" {
    
    unsigned int PLUGIN_CALL Supports() {
        return SUPPORTS_VERSION | SUPPORTS_AMX_NATIVES;
    }

    // Eklenti yüklendiğinde çalışır
    bool PLUGIN_CALL Load(void **ppData) {
        pAMXFunctions = ppData[PLUGIN_DATA_AMX_EXPORTS];
        
        // Log fonksiyonunu alıyoruz
        logprintf = (logprintf_t)ppData[PLUGIN_DATA_LOGPRINTF];

        logprintf(" >> samp-mongo eklentisi basariyla yuklendi! (v0.1)");
        return true;
    }

    void PLUGIN_CALL Unload() {
        logprintf(" >> samp-mongo eklentisi kapatiliyor.");
    }

    void PLUGIN_CALL ProcessTick() {
    }
}