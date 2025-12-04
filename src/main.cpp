#include <amx/amx.h>
#include <plugincommon.h>

#include <mongocxx/client.hpp>
#include <mongocxx/instance.hpp>
#include <mongocxx/uri.hpp>

#include <bsoncxx/builder/stream/document.hpp>
#include <bsoncxx/builder/stream/helpers.hpp>
#include <bsoncxx/json.hpp>

typedef void (*logprintf_t)(char* format, ...);
logprintf_t logprintf;
void *pAMXFunctions;

mongocxx::instance instance{}; 

extern "C" {
    
    unsigned int PLUGIN_CALL Supports() {
        return SUPPORTS_VERSION | SUPPORTS_AMX_NATIVES;
    }

    bool PLUGIN_CALL Load(void **ppData) {
        pAMXFunctions = ppData[PLUGIN_DATA_AMX_EXPORTS];
        logprintf = (logprintf_t)ppData[PLUGIN_DATA_LOGPRINTF];

        logprintf(" >> samp-mongo: Eklenti yukleniyor...");

        try {
            mongocxx::client client{mongocxx::uri{}};

            auto admin_db = client["admin"];
            
            auto result = admin_db.run_command(
                bsoncxx::builder::stream::document{} 
                << "ping" << 1 
                << bsoncxx::builder::stream::finalize
            );
            
            logprintf(" >> samp-mongo: MongoDB baglantisi BASARILI! (Ping: OK)");

        } catch (const std::exception& e) {
            logprintf(" >> samp-mongo HATASI: MongoDB baglantisi kurulamadi!");
            logprintf(" >> Hata Detayi: %s", e.what());
            return true; 
        }

        return true;
    }

    void PLUGIN_CALL Unload() {
        logprintf(" >> samp-mongo: Eklenti kapatildi.");
    }

    void PLUGIN_CALL ProcessTick() {
    }
}