
# samp-mongo (v1.0.0)

A high-performance, asynchronous, and thread-safe MongoDB plugin for SA-MP, written in C++17.

## Features
- **True Async:** Non-blocking background threads.
- **Full CRUD:** Insert, Find, Update, Delete support.
- **Modular:** Clean `MG_*` API design.
- **Stable:** Mutex-locked task queue to prevent crashes.

## Installation
1. Put `samp-mongo.dll` in `plugins/`.
2. Put `samp-mongo.inc` in `pawno/include/`.
3. Add `plugins samp-mongo` to `server.cfg`.

## Usage Examples

### 1. Connect
```C++
#include <a_samp>
#include <samp-mongo>

new DB;

public OnGameModeInit() {
    DB = MG_Connect("mongodb://localhost:27017", "MyGameDB");
    return 1;
}
```
### 2. Insert (Create)

Kod snippet'i

```C++
MG_CreateDoc();
MG_AddString("Username", "Player1");
MG_AddInt("Score", 100);
MG_QueryAsync(DB, "Users", MG_INSERT);

```

### 3. Find (Read)

Kod snippet'i

```C++
MG_CreateDoc();
MG_AddString("Username", "Player1"); // Filter
MG_QueryAsync(DB, "Users", MG_FIND, "OnUserLoaded");

forward OnUserLoaded(resultid, playerid);
public OnUserLoaded(resultid, playerid) {
    while(MG_ResultNext(resultid)) {
        new name[24], score;
        MG_GetResultString(resultid, "Username", name);
        score = MG_GetResultInt(resultid, "Score");
        printf("User: %s, Score: %d", name, score);
    }
    MG_FreeResult(resultid); // Important!
    return 1;
}
```
### 4. Update

Kod snippet'i

```C++
// Data to update
MG_CreateDoc();
MG_AddInt("Score", 500);

// Update "Users" where "Username" is "Player1"
MG_UpdateAsync(DB, "Users", "Username", "Player1");

```

### 5. Delete

Kod snippet'i

```C++
// Filter
MG_CreateDoc();
MG_AddString("Username", "Player1");
MG_QueryAsync(DB, "Users", MG_DELETE);

```

## Build

Requires **Visual Studio 2022**, **CMake**, and **vcpkg**.

Bash

```
vcpkg install mongo-cxx-driver:x86-windows-static
cmake -S . -B build -G "Visual Studio 17 2022" -A Win32 -DCMAKE_TOOLCHAIN_FILE=[VCPKG_PATH]/scripts/buildsystems/vcpkg.cmake -DVCPKG_TARGET_TRIPLET=x86-windows-static
cmake --build build --config Release
```
