
#include "wendfyr/bootstrap.hpp"
#include "wendfyr/services/logging.hpp"

#include <ftxui/component/component.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/dom/node.hpp>
#include <spdlog/spdlog.h>

#include <cstdlib>
#include <filesystem>

#include "command_bar.hpp"
#include "input_handler.hpp"
#include "panel_view.hpp"

using namespace ftxui;

int main()
{
    try
    {
        wendfyr::services::logging::initLogging(
            {.console_level = wendfyr::services::logging::LogLevel::OFF,
             .enable_file = true});

        auto home{std::filesystem::current_path()};
        auto ctx{wendfyr::createApplication(home)};

        wendfyr::tui::PanelView left_view{*ctx.left_panel};
        wendfyr::tui::PanelView right_view{*ctx.right_panel};

        auto screen{ScreenInteractive::Fullscreen()};

        wendfyr::tui::InputHandler inp(left_view, right_view,
                                       *ctx.command_executor,
                                       *ctx.command_factory, screen);
        auto renderer{Renderer(
            [&]
            {
                return vbox({
                    hbox({
                        left_view.render(inp.isLeftActive()),
                        right_view.render(!inp.isLeftActive()),
                    }) | flex,
                    wendfyr::tui::renderCommandBar(),
                });
            })};

        auto component{CatchEvent(renderer, [&](const Event& event) -> bool
                                  { return inp.handle(event); })};

        screen.Loop(component);

        return EXIT_SUCCESS;
    }
    catch (const std::exception& e)
    {
        spdlog::error(std::format("Fatal error: {}", e.what()));
        return EXIT_FAILURE;
    }
}
