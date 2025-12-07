#include "worker.h"
#include <bsoncxx/json.hpp>

using bsoncxx::builder::stream::document;
using bsoncxx::builder::stream::finalize;

// Global Değişkenler
mongocxx::instance g_Instance{};
std::vector<mongocxx::client*> g_Clients;
std::vector<mongocxx::database> g_Databases;
std::unique_ptr<bsoncxx::builder::stream::document> g_BsonBuilder;

// Threading
std::queue<Task*> g_InputQueue;
std::queue<Task*> g_OutputQueue;
std::mutex g_InputMutex;
std::mutex g_OutputMutex;
std::thread g_Thread;
bool g_Running = false;

extern void Log(const char* fmt, ...);

extern int ExecCallback(AMX* amx, const char* func, int playerid, int resultIdx);

void WorkerLoop() {
    while (g_Running) {
        Task* task = nullptr;

        // 1. İş Al
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
                    auto cursor = col.find(task->filter ? task->filter->view() : document{}.view());
                    for (auto&& doc : cursor) {
                        task->results.push_back(bsoncxx::document::value(doc));
                    }
                }
            }
        } catch (const std::exception& e) {
            // Log("MongoDB Error: %s", e.what());
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

std::vector<Task*> g_ResultCache;

void ProcessCallbacks() {
    std::lock_guard<std::mutex> lock(g_OutputMutex);
    while (!g_OutputQueue.empty()) {
        Task* task = g_OutputQueue.front();
        g_OutputQueue.pop();

        if (task->amxInstance) {
            // Sonucu Cache'e at (Pawn okuyabilsin diye)
            g_ResultCache.push_back(task);
            int resultIndex = g_ResultCache.size() - 1;

            // Callback Çağır
            ExecCallback(task->amxInstance, task->callbackFunc.c_str(), task->playerID, resultIndex);
            
            // İş bitti, silinecek ama hemen değil (GetStr yapabilsinler diye)
            // Gerçek projede ResultCache temizleme mekanizması gerekir.
            // Şimdilik basit tutuyoruz.
        } else {
            delete task;
        }
    }
}