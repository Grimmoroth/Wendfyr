#include "input_handler.hpp"

#include "wendfyr/domain/models/file_entry.hpp"
#include "wendfyr/ports/driving/i_command_executor.hpp"
#include "wendfyr/ports/driving/i_command_factory.hpp"

#include <ftxui/component/screen_interactive.hpp>
#include <spdlog/spdlog.h>

#include "panel_view.hpp"

using namespace ftxui;
#include <format>
namespace wendfyr::tui
{
    InputHandler::InputHandler(PanelView& left_panel,
                               PanelView& right_panel,
                               ports::driving::ICommandExecutor& executor,
                               ports::driving::ICommandFactory& factory,
                               ftxui::ScreenInteractive& screen)
        : _left_panel{left_panel}
        , _right_panel{right_panel}
        , _executor{executor}
        , _factory{factory}
        , _screen{screen}
    {
    }

    bool InputHandler::handle(const Event& event)
    {
        if (event == Event::Character('q') ||
            event == Event::Character('Q'))
        {
            _screen.Exit();
            return true;
        }

        if (event == Event::Tab)
        {
            handleSwitchPanel();
            return true;
        }

        if (event == Event::ArrowDown || event == Event::ArrowUp ||
            event == Event::Home || event == Event::End)
        {
            handleNavigation(event);
            return true;
        }

        if (event == Event::Return)
        {
            handleEnterDirectory();
            return true;
        }

        if (event == Event::Backspace)
        {
            handleNavigateUp();
            return true;
        }

        if (event == Event::Character(' '))
        {
            handleToggleSelection();
            return true;
        }

        if (event == Event::F5)
        {
            handleCopy();
            return true;
        }

        if (event == Event::F6)
        {
            handleMove();
            return true;
        }

        if (event == Event::F7)
        {
            handleMkdir();
            return true;
        }

        if (event == Event::F8)
        {
            handleDelete();
            return true;
        }

        if (event == Event::Special("\x1A"))
        {
            handleUndo();
            return true;
        }

        if (event == Event::Special("\x19"))
        {
            handleRedo();
            return true;
        }

        return false;
    }

    bool InputHandler::isLeftActive() const noexcept
    {
        return _left_active;
    }

    PanelView& InputHandler::activePanel() noexcept
    {
        return _left_active ? _left_panel : _right_panel;
    }

    PanelView& InputHandler::inactivePanel() noexcept
    {
        return _left_active ? _right_panel : _left_panel;
    }

    void InputHandler::handleSwitchPanel()
    {
        _left_active = !_left_active;
    }

    void InputHandler::handleNavigation(const Event& event)
    {
        auto& panel{activePanel()};

        if (event == Event::ArrowDown)
        {
            panel.cursorDown();
        }
        else if (event == Event::ArrowUp)
        {
            panel.cursorUp();
        }
        else if (event == Event::Home)
        {
            panel.cursorToTop();
        }
        else if (event == Event::End)
        {
            panel.cursorToBottom();
        }
    }

    void InputHandler::handleEnterDirectory()
    {
        auto& panel{activePanel()};
        const auto* entry{panel.currentEntry()};
        if (entry == nullptr)
        {
            return;
        }

        if (entry->type == domain::models::EntryType::DIRECTORY)
        {
            panel.navigateTo(entry->path);
        }
    }

    void InputHandler::handleNavigateUp()
    {
        activePanel().navigateUp();
    }

    void InputHandler::handleToggleSelection()
    {
        activePanel().toogleSelection();
    }

    std::vector<std::filesystem::path> InputHandler::collectSourcePaths()
    {
        auto& panel{activePanel()};
        auto selected{panel.selectedEntries()};

        if (!selected.empty())
        {
            std::vector<std::filesystem::path> paths;
            paths.reserve(selected.size());

            for (const auto& entry : selected)
            {
                paths.push_back(entry.path);
            }
            return paths;
        }

        const auto* entry{panel.currentEntry()};
        if (entry != nullptr)
        {
            return {entry->path};
        }

        return {};
    }

    void InputHandler::handleCopy()
    {
        auto sources{collectSourcePaths()};
        if (sources.empty())
        {
            return;
        }

        auto dest{inactivePanel().currentDirectory()};

        try
        {
            _executor.execute(
                _factory.createCopyCommand(std::move(sources), dest));
            activePanel().deselectAll();
        }
        catch (const std::exception& e)
        {
            spdlog::error(std::format("Copy failed: {}", e.what()));
        }
    }

    void InputHandler::handleMove()
    {
        auto sources{collectSourcePaths()};
        if (sources.empty())
        {
            return;
        }

        auto dest{inactivePanel().currentDirectory()};

        try
        {
            _executor.execute(
                _factory.createMoveCommand(std::move(sources), dest));
            activePanel().deselectAll();
            activePanel().resetCursor();
        }
        catch (const std::exception& e)
        {
            spdlog::error(std::format("Move failed: {}", e.what()));
        }
    }

    void InputHandler::handleMkdir()
    {
        auto new_dir{activePanel().currentDirectory() / "New Folder"};
        try
        {
            _executor.execute(
                _factory.createCreateDirectoryCommand(new_dir));
        }
        catch (const std::exception& e)
        {
            spdlog::error(std::format("Mkdir failed: {}", e.what()));
        }
    }

    void InputHandler::handleDelete()
    {
        auto targets{collectSourcePaths()};

        if (targets.empty())
        {
            return;
        }

        try
        {
            _executor.execute(
                _factory.createDeleteCommand(std::move(targets)));
            activePanel().deselectAll();
            activePanel().resetCursor();
        }
        catch (const std::exception& e)
        {
            spdlog::error(std::format("Delete failed: {}", e.what()));
        }
    }

    void InputHandler::handleUndo()
    {
        if (!_executor.canUndo())
        {
            return;
        }

        try
        {
            _executor.undo();
        }
        catch (const std::exception& e)
        {
            spdlog::error(std::format("Undo failed: {}", e.what()));
        }
    }

    void InputHandler::handleRedo()
    {
        if (!_executor.canRedo())
        {
            return;
        }

        try
        {
            _executor.redo();
        }
        catch (const std::exception& e)
        {
            spdlog::error(std::format("Redo failed: {}", e.what()));
        }
    }

};  // namespace wendfyr::tui
