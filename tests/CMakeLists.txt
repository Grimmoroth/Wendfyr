if(BUILD_TESTING)
    add_executable(wf_integration_tests
        integration_test.cpp
    )

    target_link_libraries(wf_integration_tests
        PRIVATE
            Wendfyr::Core
            GTest::gtest
            GTest::gtest_main
            wf_warnings
    )

    include(GoogleTest)
    gtest_discover_tests(wf_integration_tests)
endif()