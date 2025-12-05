#include <amx/amx.h>
#include <plugincommon.h>

#include <string>
#include <vector>
#include <memory>
#include <mongocxx/client.hpp>
#include <mongocxx/instance.hpp>
#include <mongocxx/uri.hpp>
#include <bsoncxx/json.hpp>
#include <bsoncxx/builder/stream/document.hpp>
#include <bsoncxx/builder/stream/helpers.hpp>
#include <bsoncxx/types.hpp>

using bsoncxx::builder::stream::document;
using bsoncxx::builder::stream::finalize;
using bsoncxx::builder::stream::open_document;
using bsoncxx::builder::stream::close_document;

typedef void (*logprintf_t)(char* format, ...);
logprintf_t logprintf;
void *pAMXFunctions;

struct MongoConnection {
    mongocxx::client client;
    mongocxx::database db;
    bool isConnected;
};

struct SearchResult {
    std::vector<bsoncxx::document::value> documents;
    int currentIndex;
};

mongocxx::instance g_Instance{};
std::vector<MongoConnection*> g_Connections;
std::vector<SearchResult*> g_Results;

std::unique_ptr<bsoncxx::builder::stream::document> g_Builder;

std::string GetString(AMX* amx, cell param) {
    cell* addr = NULL;
    int len = 0;
    amx_GetAddr(amx, param, &addr);
    amx_StrLen(addr, &len);
    if (len == 0) return std::string("");
    char* text = new char[len + 1];
    amx_GetString(text, addr, 0, len + 1);
    std::string result(text);
    delete[] text;
    return result;
}

extern "C" {

    unsigned int PLUGIN_CALL Supports() {
        return SUPPORTS_VERSION | SUPPORTS_AMX_NATIVES;
    }

    bool PLUGIN_CALL Load(void **ppData) {
        pAMXFunctions = ppData[PLUGIN_DATA_AMX_EXPORTS];
        logprintf = (logprintf_t)ppData[PLUGIN_DATA_LOGPRINTF];
        return true;
    }

    void PLUGIN_CALL Unload() {
        for (auto conn : g_Connections) delete conn;
        for (auto res : g_Results) delete res;
        g_Connections.clear();
        g_Results.clear();
    }

    cell AMX_NATIVE_CALL Mongo_Connect(AMX* amx, cell* params) {
        std::string uri_str = GetString(amx, params[1]);
        std::string db_name = GetString(amx, params[2]);
        try {
            MongoConnection* newConn = new MongoConnection();
            newConn->client = mongocxx::client{mongocxx::uri{uri_str}};
            newConn->db = newConn->client[db_name];
            newConn->db.run_command(document{} << "ping" << 1 << finalize);
            newConn->isConnected = true;
            g_Connections.push_back(newConn);
            return g_Connections.size(); 
        } catch (const std::exception& e) {
            logprintf("[Mongo] Connection Error: %s", e.what());
            return 0;
        }
    }

    cell AMX_NATIVE_CALL Mongo_NewDocument(AMX* amx, cell* params) {
        g_Builder = std::make_unique<bsoncxx::builder::stream::document>();
        return 1;
    }

    cell AMX_NATIVE_CALL Mongo_AddString(AMX* amx, cell* params) {
        if (!g_Builder) return 0;
        std::string key = GetString(amx, params[1]);
        std::string value = GetString(amx, params[2]);
        (*g_Builder) << key << value;
        return 1;
    }

    cell AMX_NATIVE_CALL Mongo_AddInt(AMX* amx, cell* params) {
        if (!g_Builder) return 0;
        std::string key = GetString(amx, params[1]);
        int value = (int)params[2];
        (*g_Builder) << key << value;
        return 1;
    }

    cell AMX_NATIVE_CALL Mongo_AddFloat(AMX* amx, cell* params) {
        if (!g_Builder) return 0;
        std::string key = GetString(amx, params[1]);
        float value = amx_ctof(params[2]);
        (*g_Builder) << key << value;
        return 1;
    }

    cell AMX_NATIVE_CALL Mongo_Insert(AMX* amx, cell* params) {
        if (!g_Builder) return 0;
        int connId = params[1];
        std::string collection_name = GetString(amx, params[2]);

        if (connId < 1 || connId > g_Connections.size()) return 0;

        try {
            auto collection = g_Connections[connId - 1]->db[collection_name];
            collection.insert_one(g_Builder->view());
            g_Builder.reset(); 
            return 1;
        } catch (const std::exception& e) {
            logprintf("[Mongo] Insert Error: %s", e.what());
            return 0;
        }
    }

    cell AMX_NATIVE_CALL Mongo_Find(AMX* amx, cell* params) {
        int connId = params[1];
        std::string collection_name = GetString(amx, params[2]);

        if (connId < 1 || connId > g_Connections.size()) return 0;

        try {
            auto collection = g_Connections[connId - 1]->db[collection_name];
            auto empty_doc = document{}; 
            bsoncxx::document::view filter_view = empty_doc.view();
            if (g_Builder) filter_view = g_Builder->view();
            
            auto cursor = collection.find(filter_view);
            SearchResult* res = new SearchResult();
            for (auto&& doc : cursor) {
                res->documents.push_back(bsoncxx::document::value(doc));
            }
            res->currentIndex = -1;
            g_Results.push_back(res);
            g_Builder.reset();
            return g_Results.size();
        } catch (const std::exception& e) {
            logprintf("[Mongo] Find Error: %s", e.what());
            return 0;
        }
    }

    cell AMX_NATIVE_CALL Mongo_Next(AMX* amx, cell* params) {
        int resId = params[1];
        if (resId < 1 || resId > g_Results.size()) return 0;
        SearchResult* res = g_Results[resId - 1];
        if (!res) return 0;
        res->currentIndex++;
        if (res->currentIndex >= res->documents.size()) return 0;
        return 1;
    }

    cell AMX_NATIVE_CALL Mongo_FreeResult(AMX* amx, cell* params) {
        int resId = params[1];
        if (resId < 1 || resId > g_Results.size()) return 0;
        SearchResult* res = g_Results[resId - 1];
        if (res) {
            delete res;
            g_Results[resId - 1] = nullptr;
        }
        return 1;
    }

    cell AMX_NATIVE_CALL Mongo_GetStr(AMX* amx, cell* params) {
        int resId = params[1];
        std::string key = GetString(amx, params[2]);
        if (resId < 1 || resId > g_Results.size()) return 0;
        SearchResult* res = g_Results[resId - 1];
        try {
            auto element = res->documents[res->currentIndex].view()[key];
            if (element && element.type() == bsoncxx::type::k_string) {
                std::string val = std::string(element.get_string().value);
                cell* addr = NULL;
                amx_GetAddr(amx, params[3], &addr);
                amx_SetString(addr, val.c_str(), 0, 0, params[4]);
                return 1;
            }
        } catch(...) {}
        return 0;
    }

    cell AMX_NATIVE_CALL Mongo_GetInt(AMX* amx, cell* params) {
        int resId = params[1];
        std::string key = GetString(amx, params[2]);
        if (resId < 1 || resId > g_Results.size()) return 0;
        SearchResult* res = g_Results[resId - 1];
        try {
            auto element = res->documents[res->currentIndex].view()[key];
            if (element) {
                if (element.type() == bsoncxx::type::k_int32) return element.get_int32().value;
                if (element.type() == bsoncxx::type::k_int64) return (int)element.get_int64().value;
                if (element.type() == bsoncxx::type::k_double) return (int)element.get_double().value;
            }
        } catch(...) {}
        return 0;
    }

    cell AMX_NATIVE_CALL Mongo_Update(AMX* amx, cell* params) {
        if (!g_Builder) return 0;
        int connId = params[1];
        std::string collection_name = GetString(amx, params[2]);
        std::string filter_key = GetString(amx, params[3]);
        std::string filter_val = GetString(amx, params[4]);

        if (connId < 1 || connId > g_Connections.size()) return 0;

        try {
            auto collection = g_Connections[connId - 1]->db[collection_name];
            
            // Filtre: { "Key": "Value" }
            auto filter = document{} << filter_key << filter_val << finalize;
            
            // Guncelleme: { "$set": { ...Builder... } }
            auto update = document{} << "$set" << g_Builder->view() << finalize;

            auto result = collection.update_one(filter.view(), update.view());
            g_Builder.reset();
            
            return result ? result->modified_count() : 0;
        } catch (const std::exception& e) {
            logprintf("[Mongo] Update Error: %s", e.what());
            return 0;
        }
    }

    cell AMX_NATIVE_CALL Mongo_Delete(AMX* amx, cell* params) {
        if (!g_Builder) return 0;
        int connId = params[1];
        std::string collection_name = GetString(amx, params[2]);

        if (connId < 1 || connId > g_Connections.size()) return 0;

        try {
            auto collection = g_Connections[connId - 1]->db[collection_name];
            
            // Builder icindeki filtreye uyanlari sil
            auto result = collection.delete_one(g_Builder->view());
            g_Builder.reset();
            
            return result ? result->deleted_count() : 0;
        } catch (const std::exception& e) {
            logprintf("[Mongo] Delete Error: %s", e.what());
            return 0;
        }
    }

    AMX_NATIVE_INFO natives[] = {
        {"Mongo_Connect", Mongo_Connect},
        {"Mongo_NewDocument", Mongo_NewDocument},
        {"Mongo_AddString", Mongo_AddString},
        {"Mongo_AddInt", Mongo_AddInt},
        {"Mongo_AddFloat", Mongo_AddFloat},
        {"Mongo_Insert", Mongo_Insert},
        {"Mongo_Find", Mongo_Find},
        {"Mongo_Next", Mongo_Next},
        {"Mongo_FreeResult", Mongo_FreeResult},
        {"Mongo_GetStr", Mongo_GetStr},
        {"Mongo_GetInt", Mongo_GetInt},
        {"Mongo_Update", Mongo_Update}, // YENI
        {"Mongo_Delete", Mongo_Delete}, // YENI
        {0, 0}
    };

    int PLUGIN_CALL AmxLoad(AMX* amx) {
        return amx_Register(amx, natives, -1);
    }

    int PLUGIN_CALL AmxUnload(AMX* amx) {
        return AMX_ERR_NONE;
    }

    void PLUGIN_CALL ProcessTick() {}
}