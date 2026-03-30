cmake_minimum_required(VERSION 3.25)

project(
    Wendfyr
    VERSION 0.1.0
    LANGUAGES CXX
    DESCRIPTION "A dual pane file manager"
    HOMEPAGE_URL "https://github.com/Grimmoroth/Wendfyr"
)

if(CMAKE_SOURCE_DIR STREQUAL CMAKE_BINARY_DIR)
    message(FATAL_ERROR
        "In-source builds are forbidden.\n"
        "Create a build directory: cmake -B build -S ."
    )
endif()

if(PROJECT_IS_TOP_LEVEL)

    list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

    if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
        message(STATUS "No build type specified. Defaulting to Debug.")
        set(CMAKE_BUILD_TYPE Debug CACHE STRING "Build type" FORCE)
        set_property(
            CACHE CMAKE_BUILD_TYPE
            PROPERTY STRINGS "Debug" "Release" "RelWithDebInfo" "MinSizeRel"
        )
    endif()

    option(BUILD_TESTING "Build the test suite" ON)
    set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

endif()

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXSTENSIONS OFF)

find_package(spdlog REQUIRED)
find_package(nlohmann_json REQUIRED)
find_package(CLI11 REQUIRED)
find_package(ftxui REQUIRED)

include(CompilerWarnings)

function(wendfyr_add_library TARGET_NAME)
    cmake_parse_arguments(
        LIB
        ""
        ""
        "SOURCES;DEPENDENCIES"
        ${ARGN}
    )

    add_library(${TARGET_NAME} STATIC ${LIB_SOURCES})
    target_include_directories(${TARGET_NAME}
        PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
    )

    if(LIB_DEPENDENCIES)
        target_link_libraries(${TARGET_NAME}
            PUBLIC ${LIB_DEPENDENCIES}
        )
    endif()

    target_link_libraries($(TARGET_NAME)
        PRIVATE wendfyr_warnings
    )

endfunction()

add_subdirectory(src)

if(BUILD_TESTING AND PROJECT_IS_TOP_LEVEL)
    enable_testing()
    find_package(GTest REQUIRED)
    add_subdirectory(tests)
endif

    
