cmake_minimum_required(VERSION 3.15)
project(samp-mongo CXX)
set(CMAKE_CXX_STANDARD 17)

if(MSVC)
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
endif()

find_package(mongocxx CONFIG REQUIRED)
find_package(bsoncxx CONFIG REQUIRED)

# TÜM DOSYALARI EKLE
add_library(samp-mongo SHARED 
    src/main.cpp 
    src/worker.cpp 
    src/natives.cpp
    lib/samp-sdk/amx/amxplugin.cpp
    samp-mongo.def
)

# DEF DOSYASINI ZORLA
if(MSVC)
    set_target_properties(samp-mongo PROPERTIES LINK_FLAGS "/DEF:\"${CMAKE_CURRENT_SOURCE_DIR}/samp-mongo.def\"")
endif()

target_compile_definitions(samp-mongo PRIVATE HAVE_STDINT_H MONGOCXX_STATIC BSONCXX_STATIC)
target_include_directories(samp-mongo PRIVATE lib/samp-sdk src)
target_link_libraries(samp-mongo PRIVATE mongo::mongocxx_static mongo::bsoncxx_static)