cmake_minimum_required(VERSION 3.20.0)
set(PROJECT_NAME container_test)
project(${PROJECT_NAME} LANGUAGES C CXX VERSION 0.1.0)

set(CMAKE_C_STANDARD_REQUIRED on)
set(CMAKE_C_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED on)
set(CMAKE_CXX_STANDARD 20)

# set(CMAKE_BUILD_TYPE Release)
set(CMAKE_BUILD_TYPE Debug)
set(PLATFORM x64)
set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_C_FLAGS -Wall)
set(CMAKE_CXX_FLAGS -Wall)

set(BASE_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(SOURCE_DIR ${BASE_DIR}/src)
set(BUILD_DIR ${BASE_DIR}/build)
set(EXECUTABLE_OUTPUT_PATH ${BUILD_DIR}/${PLATFORM}/${CMAKE_BUILD_TYPE}/bin)


file(GLOB SOURCES ${SOURCE_DIR}/*.c ${SOURCE_DIR}/*.cpp)
include_directories(
    ${BASE_DIR}/../lib/googletest/googletest/include
    ${BASE_DIR}/../lib/googletest/googmock/include
    ${BASE_DIR}/../include
)

link_directories(
    ${BASE_DIR}/../lib
    ${BASE_DIR}/../lib/googletest/build/lib
)


add_executable(${PROJECT_NAME} ${SOURCES})
if (${CMAKE_BUILD_TYPE} STREQUAL "Debug")
    add_compile_definitions(
        __TEST__
    )

    add_compile_options(-O1 -g)
else ()
    add_compile_options(-O3)
endif ()
target_compile_definitions(${PROJECT_NAME}
    PUBLIC __cp
)

target_link_libraries(${PROJECT_NAME} gtest gmock gtest_main gmock_main pthread container)





