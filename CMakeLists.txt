cmake_minimum_required(VERSION 3.16)
project(MOSCO VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(MSVC)
    add_compile_options(/W4)
else()
    add_compile_options(-Wall -Wextra -Wpedantic -Wno-unused-parameter)  # ← Добавлен -Wno-unused-parameter
endif()

option(BUILD_TESTS "Build test executable" ON)

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
    include/graph.cpp
    include/graph_serializer.cpp
)

set(HEADERS
    include/graph.h
    include/graph_serializer.h
)

add_executable(mosco ${SOURCES} ${HEADERS})

target_include_directories(mosco PRIVATE 
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# header-only библиотека
target_include_directories(mosco PRIVATE 
    ${json_SOURCE_DIR}/include
)

set_target_properties(mosco PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

if(BUILD_TESTS)
    enable_testing()
    add_test(NAME MOSCOTest COMMAND mosco)
endif()

message(STATUS "C++ Standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "Build tests: ${BUILD_TESTS}")
message(STATUS "JSON include: ${json_SOURCE_DIR}/include")