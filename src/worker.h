#pragma once
#include <string>
#include <vector>
#include <queue>
#include <mutex>
#include <thread>
#include <optional>
#include <amx/amx.h>

#include <mongocxx/client.hpp>
#include <mongocxx/instance.hpp>
#include <bsoncxx/builder/stream/document.hpp>

enum class RequestType { INSERT, UPDATE, DELETE, FIND };

struct Task {
    RequestType type;
    int connectionID;
    std::string collection;
    std::optional<bsoncxx::document::value> data;
    std::optional<bsoncxx::document::value> filter;
    
    bool hasCallback;
    std::string callbackFunc;
    int playerID;
    AMX* amxInstance;

    std::vector<bsoncxx::document::value> results;
    int currentIter = -1;
};

// Global Veriler
extern std::unique_ptr<bsoncxx::builder::stream::document> g_BsonBuilder;
extern std::vector<mongocxx::client*> g_Clients;
extern std::vector<mongocxx::database> g_Databases;
extern std::vector<Task*> g_ResultCache;

// Fonksiyonlar
void StartWorker();
void StopWorker();
void AddTask(Task* task);
void ProcessCallbacks();