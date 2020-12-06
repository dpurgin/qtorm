cmake_minimum_required(VERSION 3.10)

project(QtOrm LANGUAGES CXX)

option(QTORM_BUILD_EXAMPLES "Build QtOrm examples" OFF)
option(QTORM_BUILD_TESTS "Build QtOrm tests" OFF)
option(QTORM_BUILD_SHARED_LIBS "Build QtOrm as shared library (LGPLv3)" ON)

message("QtOrm Configuration:")
message("    Examples: ${QTORM_BUILD_EXAMPLES}")
message("    Tests: ${QTORM_BUILD_TESTS}")
message("    Shared libs (LGPLv3): ${QTORM_BUILD_SHARED_LIBS}")

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON) 

find_package(Qt5 COMPONENTS Core Sql REQUIRED)

add_subdirectory(src)

if (QTORM_BUILD_EXAMPLES)
    add_subdirectory(examples)
endif() 

if (QTORM_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()
