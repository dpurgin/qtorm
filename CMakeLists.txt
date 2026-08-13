cmake_minimum_required(VERSION 3.10)

project(QtOrm LANGUAGES CXX)

set(QTORM_VERSION_MAJOR 1)
set(QTORM_VERSION_MINOR 0)
set(QTORM_VERSION_PATCH 1)

# https://stackoverflow.com/questions/25199677/how-to-detect-if-current-scope-has-a-parent-in-cmake
get_directory_property(hasParent PARENT_DIRECTORY)

if (hasParent)
    option(QTORM_BUILD_EXAMPLES "Build QtOrm examples" OFF)
    option(QTORM_BUILD_TESTS "Build QtOrm tests" OFF)
else()
    option(QTORM_BUILD_EXAMPLES "Build QtOrm examples" ON)
    option(QTORM_BUILD_TESTS "Build QtOrm tests" ON)
endif()

option(QTORM_BUILD_SHARED_LIBS "Build QtOrm as shared library (LGPLv3)" ON)
set(QTORM_QT_VERSION_HINT "auto" CACHE STRING "Qt version to use (5, 6, or auto)")

if (QTORM_QT_VERSION_HINT STREQUAL "auto")
    find_package(Qt6 COMPONENTS Core QUIET)

    if (Qt6_FOUND)
        set(QTORM_QT_VERSION_MAJOR 6)
        set(QTORM_QT_SUFFIX -qt6)
        set(QTORM_PKGCONFIG_DEPS "Qt6Core")
    else()
        set(QTORM_QT_VERSION_MAJOR 5)
        set(QTORM_QT_SUFFIX "")
        set(QTORM_PKGCONFIG_DEPS "Qt5Core")
    endif()
elseif (QTORM_QT_VERSION_HINT STREQUAL "5")
    set(QTORM_QT_VERSION_MAJOR 5)
    set(QTORM_QT_SUFFIX "")
    set(QTORM_PKGCONFIG_DEPS "Qt5Core")
else()
    set(QTORM_QT_VERSION_MAJOR 6)
    set(QTORM_QT_SUFFIX -qt6)
    set(QTORM_PKGCONFIG_DEPS "Qt6Core")
endif()

message("QtOrm Configuration:")
message("    Examples: ${QTORM_BUILD_EXAMPLES}")
message("    Tests: ${QTORM_BUILD_TESTS}")
message("    Shared libs (LGPLv3): ${QTORM_BUILD_SHARED_LIBS}")
message("    Qt version: ${QTORM_QT_VERSION_HINT}, detected ${QTORM_QT_VERSION_MAJOR}")

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON) 

find_package(Qt${QTORM_QT_VERSION_MAJOR} COMPONENTS Core Sql REQUIRED)

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
