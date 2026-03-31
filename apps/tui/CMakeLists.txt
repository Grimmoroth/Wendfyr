add_executable(wf_tui
    src/main.cpp
)

target_link_libraries(wf_tui
    PRIVATE
        Wendfyr::Core
        ftxui::screen
        ftxui::dom
        ftxui::components
        wf_warnings
)

if(BUILD_TESTING)

    add_executable(wf_tui_tests
        tests/tui_adapter_test.cpp
    )

    target_link_libraries(wf_tui_tests
        Wendfyr::Core
        ftxui::screen
        ftxui::dom
        ftxui::component
        GTest::gtest
        GTest::gtest_main
        GTest::gmock
        wf_warnings
    )

    include(GoogleTest)
    gtest_discover_tests(wf_tui_tests)

endif()