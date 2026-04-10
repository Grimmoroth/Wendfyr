from conan import ConanFile
from conan.tools.cmake import CMakeToolchain,CMakeDeps, cmake_layout


class WendfyrConan(ConanFile):

    name = "Wendfyr"
    version = "0.1.0"
    description = "A dual-pane file manager with CLI, TUI, and GUI frontends"
    license = "MIT"

    settings = "os","compiler","arch","build_type"

    options = {
        "with_tui": [True,False],
        "with_gui": [True,False]
    }

    default_options = {
        "with_tui" : True,
        "with_gui" : False
    }

    def requirements(self):
        self.requires("spdlog/1.15.0")
        self.requires("nlohmann_json/3.12.0")

        self.requires("cli11/2.4.2")

        if self.options.with_tui:
            self.requires("ftxui/5.0.0")

        #if self.options.with_gui:
        #    self.requires("???")

    def build_requirements(self):
        self.test_requires("gtest/1.17.0")

    
    def layout(self):
        cmake_layout(self)

    def generate(self):
        tc = CMakeToolchain(self)

        tc.variables["CMAKE_CXX_STANDARD"] = 23
        tc.variables["CMAKE_CXX_STANDARD_REQUIRED"] = "ON"
        tc.variables["CMAKE_CXX_EXSTENSIONS"] = "OFF"

        tc.variables["WF_WITH_TUI"] = self.options.with_tui
        tc.variables["WF_WITH_GUI"] = self.options.with_gui

        tc.generate()

        deps = CMakeDeps(self)
        deps.generate()
