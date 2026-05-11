add_executable(wf_cli
    src/main.cpp
)

target_compile_features(wf_cli PRIVATE cxx_std_23)
target_precompile_headers(wf_cli REUSE_FROM Wendfyr::Core)

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
