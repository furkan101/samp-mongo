#include "worker.h"
#include "natives.h"
#include <bsoncxx/json.hpp>
#include <bsoncxx/builder/stream/document.hpp>
#include <bsoncxx/builder/stream/helpers.hpp>

using bsoncxx::builder::stream::document;
using bsoncxx::builder::stream::finalize;

mongocxx::instance g_Instance{};
std::vector<mongocxx::client*> g_Clients;
std::vector<mongocxx::database> g_Databases;
std::unique_ptr<bsoncxx::builder::stream::document> g_BsonBuilder;

std::queue<Task*> g_InputQueue;
std::queue<Task*> g_OutputQueue;
std::mutex g_InputMutex;
std::mutex g_OutputMutex;
std::thread g_Thread;
bool g_Running = false;

std::vector<Task*> g_ResultCache;

extern int ExecCallback(AMX* amx, const char* func, int playerid, int resultIdx);
extern void Log(const char* fmt, ...);

void WorkerLoop() {
    while (g_Running) {
        Task* task = nullptr;

        {
            std::lock_guard<std::mutex> lock(g_InputMutex);
            if (!g_InputQueue.empty()) {
                task = g_InputQueue.front();
                g_InputQueue.pop();
            }
        }

        if (!task) {
            std::this_thread::sleep_for(std::chrono::milliseconds(5));
            continue;
        }

        try {
            if (task->connectionID >= 1 && task->connectionID <= g_Databases.size()) {
                auto db = g_Databases[task->connectionID - 1];
                auto col = db[task->collection];

                if (task->type == RequestType::INSERT && task->data) {
                    col.insert_one(task->data->view());
                }
                else if (task->type == RequestType::FIND) {
                    auto filterView = task->filter ? task->filter->view() : document{}.view();
                    auto cursor = col.find(filterView);
                    
                    for (auto&& doc : cursor) {
                        task->results.push_back(bsoncxx::document::value(doc));
                    }
                }
                else if (task->type == RequestType::DELETE && task->filter) {
                    col.delete_one(task->filter->view());
                }
                else if (task->type == RequestType::UPDATE && task->filter && task->data) {
                    auto updateDoc = document{} << "$set" << task->data->view() << finalize;
                    col.update_one(task->filter->view(), updateDoc.view());
                }
            }
        } catch (const std::exception& e) {
            // printf("[Worker Error] %s\n", e.what());
        }

        if (task->hasCallback) {
            std::lock_guard<std::mutex> lock(g_OutputMutex);
            g_OutputQueue.push(task);
        } else {
            delete task;
        }
    }
}

void StartWorker() {
    g_Running = true;
    g_Thread = std::thread(WorkerLoop);
}

void StopWorker() {
    g_Running = false;
    if (g_Thread.joinable()) g_Thread.join();
}

void AddTask(Task* task) {
    std::lock_guard<std::mutex> lock(g_InputMutex);
    g_InputQueue.push(task);
}

void ProcessCallbacks() {
    std::lock_guard<std::mutex> lock(g_OutputMutex);
    while (!g_OutputQueue.empty()) {
        Task* task = g_OutputQueue.front();
        g_OutputQueue.pop();

        if (task->amxInstance) {
            g_ResultCache.push_back(task);
            int resultIndex = g_ResultCache.size() - 1;

            ExecCallback(task->amxInstance, task->callbackFunc.c_str(), task->playerID, resultIndex);
            
        } else {
            delete task;
        }
    }
}