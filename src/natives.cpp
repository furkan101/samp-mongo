#include "natives.h"
#include "worker.h"
#include <plugincommon.h> 
#include <bsoncxx/builder/stream/document.hpp>
#include <bsoncxx/builder/stream/helpers.hpp>

using bsoncxx::builder::stream::document;
using bsoncxx::builder::stream::finalize;

std::string GetPawnString(AMX* amx, cell param) {
    cell* addr = NULL;
    int len = 0;
    amx_GetAddr(amx, param, &addr);
    amx_StrLen(addr, &len);
    if (len == 0) return "";
    char* text = new char[len + 1];
    amx_GetString(text, addr, 0, len + 1);
    std::string res(text);
    delete[] text;
    return res;
}

Task* GetTaskResult(int resultID) {
    if (resultID < 0 || resultID >= g_ResultCache.size()) return nullptr;
    return g_ResultCache[resultID];
}

cell AMX_NATIVE_CALL MG_Connect(AMX* amx, cell* params) {
    std::string uri = GetPawnString(amx, params[1]);
    std::string dbName = GetPawnString(amx, params[2]);
    try {
        auto client = new mongocxx::client{mongocxx::uri{uri}};
        g_Clients.push_back(client);
        g_Databases.push_back(client->database(dbName));
        g_Databases.back().run_command(document{} << "ping" << 1 << finalize);
        return g_Databases.size(); 
    } catch (...) { return 0; }
}

cell AMX_NATIVE_CALL MG_CreateDoc(AMX* amx, cell* params) {
    g_BsonBuilder = std::make_unique<bsoncxx::builder::stream::document>();
    return 1;
}

cell AMX_NATIVE_CALL MG_AddString(AMX* amx, cell* params) {
    if (g_BsonBuilder) (*g_BsonBuilder) << GetPawnString(amx, params[1]) << GetPawnString(amx, params[2]);
    return 1;
}

cell AMX_NATIVE_CALL MG_AddInt(AMX* amx, cell* params) {
    if (g_BsonBuilder) (*g_BsonBuilder) << GetPawnString(amx, params[1]) << (int)params[2];
    return 1;
}

cell AMX_NATIVE_CALL MG_QueryAsync(AMX* amx, cell* params) {
    if (!g_BsonBuilder) return 0;

    Task* task = new Task();
    task->connectionID = params[1];
    task->collection = GetPawnString(amx, params[2]);
    int type = params[3];
    task->amxInstance = amx;

    std::string cbName = GetPawnString(amx, params[4]);
    if (!cbName.empty()) {
        task->hasCallback = true;
        task->callbackFunc = cbName;
        task->playerID = params[5];
    } else {
        task->hasCallback = false;
    }

    if (type == 3) {
        task->type = RequestType::FIND;
        task->filter = g_BsonBuilder->view();
    } 
    else if (type == 2) {
        task->type = RequestType::DELETE;
        task->filter = g_BsonBuilder->view();
    }
    else {
        task->type = RequestType::INSERT;
        task->data = g_BsonBuilder->view();
    }

    AddTask(task);
    g_BsonBuilder = std::make_unique<bsoncxx::builder::stream::document>(); 
    return 1;
}

cell AMX_NATIVE_CALL MG_UpdateAsync(AMX* amx, cell* params) {
    if (!g_BsonBuilder) return 0;

    Task* task = new Task();
    task->connectionID = params[1];
    task->collection = GetPawnString(amx, params[2]);
    task->type = RequestType::UPDATE;
    task->amxInstance = amx;

    std::string key = GetPawnString(amx, params[3]);
    std::string val = GetPawnString(amx, params[4]);
    task->filter = document{} << key << val << finalize;

    task->data = g_BsonBuilder->view();

    std::string cbName = GetPawnString(amx, params[5]);
    if (!cbName.empty()) {
        task->hasCallback = true;
        task->callbackFunc = cbName;
        task->playerID = params[6];
    } else {
        task->hasCallback = false;
    }

    AddTask(task);
    g_BsonBuilder = std::make_unique<bsoncxx::builder::stream::document>();
    return 1;
}

cell AMX_NATIVE_CALL MG_ResultNext(AMX* amx, cell* params) {
    Task* task = GetTaskResult(params[1]);
    if (!task) return 0;
    
    task->currentIter++;
    if (task->currentIter < task->results.size()) return 1;
    return 0;
}

cell AMX_NATIVE_CALL MG_GetResultString(AMX* amx, cell* params) {
    Task* task = GetTaskResult(params[1]);
    if (!task || task->currentIter < 0 || task->currentIter >= task->results.size()) return 0;

    std::string key = GetPawnString(amx, params[2]);
    try {
        auto doc = task->results[task->currentIter].view();
        if (doc[key] && doc[key].type() == bsoncxx::type::k_string) {
            std::string val = std::string(doc[key].get_string().value);
            cell* addr = NULL;
            amx_GetAddr(amx, params[3], &addr);
            amx_SetString(addr, val.c_str(), 0, 0, params[4]);
            return 1;
        }
    } catch (...) {}
    return 0;
}

cell AMX_NATIVE_CALL MG_GetResultInt(AMX* amx, cell* params) {
    Task* task = GetTaskResult(params[1]);
    if (!task || task->currentIter < 0 || task->currentIter >= task->results.size()) return 0;

    std::string key = GetPawnString(amx, params[2]);
    try {
        auto doc = task->results[task->currentIter].view();
        if (doc[key]) {
            if (doc[key].type() == bsoncxx::type::k_int32) return doc[key].get_int32().value;
            if (doc[key].type() == bsoncxx::type::k_int64) return (int)doc[key].get_int64().value;
            if (doc[key].type() == bsoncxx::type::k_double) return (int)doc[key].get_double().value;
        }
    } catch (...) {}
    return 0;
}

cell AMX_NATIVE_CALL MG_FreeResult(AMX* amx, cell* params) {
    int id = params[1];
    if (id >= 0 && id < g_ResultCache.size()) {
        if (g_ResultCache[id]) {
            delete g_ResultCache[id];
            g_ResultCache[id] = nullptr;
        }
    }
    return 1;
}

AMX_NATIVE_INFO PluginNatives[] = {
    {"MG_Connect", MG_Connect},
    {"MG_CreateDoc", MG_CreateDoc},
    {"MG_AddString", MG_AddString},
    {"MG_AddInt", MG_AddInt},
    {"MG_QueryAsync", MG_QueryAsync},
    {"MG_UpdateAsync", MG_UpdateAsync},
    {"MG_ResultNext", MG_ResultNext},
    {"MG_GetResultString", MG_GetResultString},
    {"MG_GetResultInt", MG_GetResultInt},
    {"MG_FreeResult", MG_FreeResult},
    {0, 0}
};