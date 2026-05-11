add_executable(wf_tui
    src/main.cpp
    src/panel_view.cpp
    src/input_handler.cpp
    src/command_bar.cpp
)

target_compile_features(wf_tui PRIVATE cxx_std_23)

target_precompile_headers(wf_tui REUSE_FROM Wendfyr::Core)

target_link_libraries(wf_tui
    PRIVATE
        Wendfyr::Core
        ftxui::screen
        ftxui::dom
        ftxui::component
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
