cmake_minimum_required(VERSION 3.16)
project(MOSCO VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(MSVC)
    add_compile_options(/W4)
else()
    add_compile_options(-Wall -Wextra -Wpedantic -Wno-unused-parameter)
endif()

include(FetchContent)

# для json
FetchContent_Declare(
    json
    GIT_REPOSITORY https://github.com/nlohmann/json.git
    GIT_TAG v3.11.3
    GIT_SHALLOW TRUE
)

FetchContent_MakeAvailable(json)

set(SOURCES
    src/main.cpp
    src/graph.cpp
    src/graph_serializer.cpp
    src/console_ui.cpp
    src/navigator_facade.cpp
    src/command.cpp
)

set(HEADERS
    include/graph.h
    include/graph_serializer.h
    include/command.h
    include/console_ui.h
    include/navigator_facade.h
    include/routing.h
)

add_executable(mosco ${SOURCES} ${HEADERS})

target_include_directories(mosco PRIVATE 
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_include_directories(mosco PRIVATE 
    ${json_SOURCE_DIR}/include
)

set_target_properties(mosco PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

message(STATUS "C++ Standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "JSON include: ${json_SOURCE_DIR}/include")