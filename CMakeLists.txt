cmake_minimum_required(VERSION 3.25)

project(
    Wendfyr
    VERSION 0.1.0
    LANGUAGES CXX
    DESCRIPTION "A dual-pane file manager with CLI, TUI, and GUI frontends"
    HOMEPAGE_URL "https://github.com/Grimmoroth/Wendfyr"
)

if(CMAKE_SOURCE_DIR STREQUAL CMAKE_BINARY_DIR)
    message(FATAL_ERROR
        "In-source builds are not allowed.\n"
        "Create a build directory and run CMake from there:\n"
        "  cmake --preset conan-debug\n"
        "Remove CMakeCache.txt and CMakeFiles/ if they were created."
    )
endif()

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXSTENSIONS OFF)


option(WF_WITH_TUI "Build the TUI frontend(requires FTXUI)" ON)
option(WF_WITH_GUI "Build the GUI frontend(toolkit TBD)"   OFF)
option(BUILD_TESTING "Build unit and integration testing"   ON)

find_package(spdlog REQUIRED)
find_package(nlohmann_json REQUIRED)
find_package(CLI11 REQUIRED)

if(WF_WITH_TUI)
    find_package(ftxui REQUIRED)
endif()

#if(WF_WITH_GUI)
#    find_package(??? REQUIRED)
#endif()

if(BUILD_TESTING)
    find_package(GTest REQUIRED)
    enable_testing()
endif()

add_subdirectory(libs/core)
add_subdirectory(apps/cli)

if(WF_WITH_TUI)
    add_subdirectory(apps/tui)
endif()

if(WF_WITH_GUI)
    add_subdirectory(apps/gui)
endif()

if(BUILD_TESTING)
    add_subdirectory(tests)
endif()
    
