#ifndef WENDFYR_TUI_INPUT_HANDLER_HPP
#define WENDFYR_TUI_INPUT_HANDLER_HPP

#include <filesystem>
#include <vector>

// #include "panel_view.hpp"
#include <ftxui/component/event.hpp>
namespace wendfyr::ports::driving
{
    class ICommandExecutor;
    class ICommandFactory;
};  // namespace wendfyr::ports::driving

namespace ftxui
{
    class ScreenInteractive;
    struct Event;
};  // namespace ftxui

namespace wendfyr::tui
{

    class PanelView;

    class InputHandler
    {
      public:
        InputHandler(PanelView& left_panel, PanelView& right_panel,
                     ports::driving::ICommandExecutor& executor,
                     ports::driving::ICommandFactory& factory,
                     ftxui::ScreenInteractive& screen);

        bool handle(const ftxui::Event& event);
        [[nodiscard]] bool isLeftActive() const noexcept;
        [[nodiscard]] PanelView& activePanel() noexcept;
        [[nodiscard]] PanelView& inactivePanel() noexcept;

      private:
        void handleNavigation(const ftxui::Event& event);
        void handleSwitchPanel();
        void handleEnterDirectory();
        void handleNavigateUp();
        void handleToggleSelection();
        void handleCopy();
        void handleMove();
        void handleMkdir();
        void handleDelete();
        void handleUndo();
        void handleRedo();

        std::vector<std::filesystem::path> collectSourcePaths();

        PanelView& _left_panel;
        PanelView& _right_panel;
        ports::driving::ICommandExecutor& _executor;
        ports::driving::ICommandFactory& _factory;
        ftxui::ScreenInteractive& _screen;

        bool _left_active{true};
    };
}  // namespace wendfyr::tui
#endif
