set(WF_CORE_SOURCES

    #Models
    src/domain/models/file_entry.cpp
    
    #Domain logic
    src/domain/command_executor.cpp
    src/domain/command_factory.cpp
    src/domain/panel_model.cpp

    #Domain concrete commands
    src/domain/commands/copy_command.cpp
    src/domain/commands/delete_command.cpp
    src/domain/commands/move_command.cpp

    src/domain/command_factory.cpp

    #Services
    src/services/event_bus.cpp

    #Infrastructore
    src/infrastructure/std_filesystem_service.cpp
    src/infrastructure/json_config_service.cpp
    
    #misc
    src/bootstrap.cpp

    #vtable anchors for the interfaces
    src/domain/commands/i_command.cpp
    src/ports/driving/i_command_executor.cpp
    src/ports/driving/i_command_factory.cpp
    src/ports/driving/i_panel_model.cpp
    src/ports/driven/i_filesystem_service.cpp
    src/ports/driven/i_config_service.cpp

)

add_library(wendfyr_core STATIC ${WF_CORE_SOURCES})

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
        tests/unit/command_executor_test.cpp
        tests/unit/panel_model_test.cpp
        tests/unit/copy_command_test.cpp
        tests/unit/move_command_test.cpp
    )

    target_include_directories(wf_core_test
        PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/tests
        ${CMAKE_CURRENT_SOURCE_DIR}/src
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