add_executable(wf_gui
    src/main.cpp
)

target_link_libraries(wf_gui
    PRIVATE
        Wendfyr::Core
        #GUI ToolKit
        wf_warnings
)

if(BUILD_TESTING)

    add_executable(wf_gui_tests
        tests/gui_adapter_test.cpp
    )

    target_link_libraries(wf_gui_tests
        PRIVATE
            Wendfyr::Core
            #GUI ToolKit
            GTest::gtest
            GTest::gtest_main
            GTest::gmock
            wf_warnings
    )

    include(GoogleTest)
    gtest_discover_tests(wf_gui_tests)

endif()