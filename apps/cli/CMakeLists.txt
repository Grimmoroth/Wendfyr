add_executable(wf_cli
    src/main.cpp
)

target_link_libraries(wf_cli
    PRIVATE
        Wendfyr::Core
        CLI11::CLI11
        wf_warnings
)

if(BUILD_TESTING)

    add_executable(wf_cli_tests
        tests/cli_adapter_test.cpp
    )

    target_link_libraries(wf_cli_tests
        PRIVATE
            Wendfyr::Core
            CLI11::CLI11
            GTest::gtest
            GTest::gtest_main
            GTest::gmock
            wf_warnings
    )

    include(GoogleTest)
    gtest_discover_tests(wf_cli_tests)

endif()