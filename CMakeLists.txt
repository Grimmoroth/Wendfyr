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
