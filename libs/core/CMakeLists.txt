set(WF_COURE_SOURCES

    #Models
    src/models/file_entry.cpp
    
    #Domain logic
    src/domain/command_executor.cpp
    src/panel_model.cpp

    #Domain concrete commands
    src/domain/commands/copy_command.cpp
    src/domain/commands/delete_command.cpp
    src/domain/commands/move_command.cpp

    #Services
    src/domain/services/event_bus.cpp

    #Infrastructore
    src/infrastructore/std_filesystem_service.cpp
    src/infrastructure/json_config_service.cpp
)

add_library(wendfyr_core STATIC ${WF_COURE_SOURCES})

target_include_directories(wendfyr_core
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(wendfyr_core
    PUBLIC
        spdlog::spdlog

    PRIVATE
        nlohmann_json::nlohmann_json
        wf_warnings
)

add_library(Wendfyr::Core ALIAS wendfyr_core)

if(BUILD_TESTING)

    add_executable(wf_core_test
        test/unit/command_executor_test.cpp
        tests/unit/panel_model_test.cpp
    )

    target_include_directories(wf_core_test
        PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/tests
    )

    target_link_libraries(wf_core_test
        PRIVATE
            Wendfyr::Core
            GTest::gtest
            GTest::gtest_main
            GTest::gmock
            wf_warnings
    )

    include(GoogleTest)
    gtest_discover_tests(wf_core_test)

endif()