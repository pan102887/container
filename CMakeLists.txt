cmake_minimum_required(VERSION 3.20.0)
set(PROJECT_NAME container)
project(${PROJECT_NAME} LANGUAGES C CXX VERSION 0.1.0)

set(CMAKE_C_STANDARD_REQUIRED on)
set(CMAKE_C_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED on)
set(CMAKE_CXX_STANDARD 20)

set(CMAKE_BUILD_TYPE Debug)
set(PLATFORM x64)

set(BASE_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(SOURCE_DIR ${BASE_DIR}/src)
set(BUILD_DIR ${BASE_DIR}/build)
set(EXECUTABLE_OUTPUT_PATH ${BUILD_DIR}/${PLATFORM}/${CMAKE_BUILD_TYPE}/bin)


file(GLOB SOURCES ${SOURCE_DIR}/*.c ${SOURCE_DIR}/*.cpp)
include_directories(${BASE_DIR}/include)
include_directories(${BASE_DIR}/src/model_a)
link_directories(${BUILD_DIR}/${PLATFORM}/${CMAKE_BUILD_TYPE}/bin)

add_executable(${PROJECT_NAME} ${SOURCES})
if (${CMAKE_BUILD_TYPE} STREQUAL "Debug")
    add_compile_definitions(
        __TEST__
    )
endif ()

# message("${SOURCES}")
# message("${CMAKE_BUILD_TYPE}")
# message("${EXECUTABLE_OUTPUT_PATH}")
# message("${BUILD_RPATH}")




