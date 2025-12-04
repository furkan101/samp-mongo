cmake_minimum_required(VERSION 3.15)

# Proje ismi
project(samp-mongo CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(MSVC)
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
endif()

find_package(mongocxx CONFIG REQUIRED)
find_package(bsoncxx CONFIG REQUIRED)

add_library(samp-mongo SHARED src/main.cpp)

target_compile_definitions(samp-mongo PRIVATE HAVE_STDINT_H)

target_compile_definitions(samp-mongo PRIVATE MONGOCXX_STATIC BSONCXX_STATIC)

target_include_directories(samp-mongo PRIVATE lib/samp-sdk)

target_link_libraries(samp-mongo PRIVATE 
    mongo::mongocxx_static 
    mongo::bsoncxx_static
)