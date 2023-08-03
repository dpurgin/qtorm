cmake_minimum_required(VERSION 3.10)

project(QtOrm LANGUAGES CXX)

# https://stackoverflow.com/questions/25199677/how-to-detect-if-current-scope-has-a-parent-in-cmake
get_directory_property(hasParent PARENT_DIRECTORY)

if (hasParent)
    option(QTORM_BUILD_EXAMPLES "Build QtOrm examples" OFF)
    option(QTORM_BUILD_TESTS "Build QtOrm tests" OFF)
    option(QTORM_BUILD_SHARED_LIBS "Build QtOrm as shared library (LGPLv3)" ON)
else()
    option(QTORM_BUILD_EXAMPLES "Build QtOrm examples" ON)
    option(QTORM_BUILD_TESTS "Build QtOrm tests" ON)
    option(QTORM_BUILD_SHARED_LIBS "Build QtOrm as shared library (LGPLv3)" ON)
endif()

message("QtOrm Configuration:")
message("    Examples: ${QTORM_BUILD_EXAMPLES}")
message("    Tests: ${QTORM_BUILD_TESTS}")
message("    Shared libs (LGPLv3): ${QTORM_BUILD_SHARED_LIBS}")

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON) 

find_package(Qt5 COMPONENTS Core Sql REQUIRED)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")

include(ExtractClassNames)
include(GetGeneratedHeaders)

add_subdirectory(src)

if (QTORM_BUILD_EXAMPLES)
    add_subdirectory(examples)
endif() 

if (QTORM_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()
