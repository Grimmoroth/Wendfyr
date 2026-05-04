#include "wendfyr/bootstrap.hpp"
#include "wendfyr/domain/models/file_entry.hpp"
#include "wendfyr/services/logging.hpp"

#include <ftxui/component/component.hpp>
#include <ftxui/component/event.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/dom/elements.hpp>
#include <spdlog/spdlog.h>

#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>

using namespace ftxui;

std::string formatSize(std::uintmax_t bytes)
{
    constexpr std::uintmax_t KB{1024};
    constexpr std::uintmax_t MB{KB * 1024};
    constexpr std::uintmax_t GB{MB * 1024};
    constexpr std::uintmax_t TB{GB * 1024};

    std::ostringstream out;
    out << std::fixed << std::setprecision(1);

    if (bytes >= TB)
    {
        out << static_cast<double>(bytes) / static_cast<double>(TB)
            << "TB";
    }

    else if (bytes >= GB)
    {
        out << static_cast<double>(bytes) / static_cast<double>(GB)
            << "GB";
    }

    else if (bytes >= MB)
    {
        out << static_cast<double>(bytes) / static_cast<double>(MB)
            << "MB";
    }

    else if (bytes >= KB)
    {
        out << static_cast<double>(bytes) / static_cast<double>(KB)
            << "KB";
    }

    else
    {
        out << bytes << "B";
    }

    return out.str();
}

Element renderPanel(wendfyr::ports::driving::IPanelModel& panel,
                    int cursor_index, bool is_active)
{
    auto entries{panel.entries()};

    auto header{hbox({
        text(" " + panel.currentDirectory().string() + " ") | bold,
    })};

    auto column_headers{
        hbox({
            text(" Name") | size(WIDTH, EQUAL, 30),
            text("Size") | size(WIDTH, EQUAL, 8) | align_right,
        }) |
        dim};

    Elements rows;
    for (int i{0}; i < static_cast<int>(entries.size()); ++i)
    {
        const auto& entry{entries[static_cast<size_t>(i)]};

        std::string prefix{"  "};
        if (entry.type == wendfyr::domain::models::EntryType::DIRECTORY)
        {
            prefix = "📁";
        }
        else if (entry.type == wendfyr::domain::models::EntryType::SYMLINK)
        {
            prefix = "🔗";
        }

        std::string size_str{};
        if (entry.type == wendfyr::domain::models::EntryType::FILE)
        {
            size_str = formatSize(entry.size);
        }

        auto name_elem{text(prefix + " " + entry.name) |
                       size(WIDTH, EQUAL, 30)};
        auto size_elem{text(size_str) | size(WIDTH, EQUAL, 8) |
                       align_right};
        auto row{hbox({name_elem, size_elem})};

        if (i == cursor_index && is_active)
        {
            row = row | inverted | focus;
        }
        else if (i == cursor_index)
        {
            row = row | inverted;
        }

        if (entry.selected)
        {
            row = row | color(Color::Yellow);
        }

        if (entry.type == wendfyr::domain::models::EntryType::DIRECTORY)
        {
            if (!entry.selected && !(i == cursor_index))
            {
                row = row | color(Color::Cyan);
            }
        }

        rows.push_back(row);
    }

    auto file_list = vbox(std::move(rows)) | vscroll_indicator | yframe;

    auto footer{
        hbox({
            text(" " + std::to_string(entries.size()) + " entries "),
        }) |
        dim};

    auto border_style = is_active ? borderHeavy : border;
    return vbox(header, separator(), column_headers, separator(),
                file_list | flex, separator(), footer) |
           border_style | flex;
}

Element renderCommandBar()
{
    return hbox(text(" F5") | bold, text(" Copy "), separator(),
                text(" F6") | bold, text(" Move "), separator(),
                text(" F7") | bold, text(" Mkdir "), separator(),
                text(" F8") | bold, text(" Delete "), separator(),
                text(" TAB") | bold, text(" SWITCH "), separator(),
                text(" Q") | bold, text(" Quit ")) |
           border;
}

int main()
{
    try
    {
        wendfyr::services::logging::initLogging(
            {.console_level = wendfyr::services::logging::LogLevel::WARN,
             .enable_console = true});

        auto home{std::filesystem::current_path()};
        auto ctx{wendfyr::createApplication(home)};

        int left_cursor{};
        int right_cursor{};
        bool left_active{true};

        auto screen{ScreenInteractive::Fullscreen()};
        auto* left_panel{ctx.left_panel.get()};
        auto* right_panel{ctx.right_panel.get()};
        auto* executor{ctx.command_executor.get()};
        auto* factory{ctx.command_factory.get()};

        auto renderer{Renderer(
            [&]
            {
                return vbox({
                    hbox({
                        renderPanel(*left_panel, left_cursor, left_active),
                        renderPanel(*right_panel, right_cursor,
                                    !left_active),
                    }) | flex,
                    renderCommandBar(),
                });
            })};

        auto component{CatchEvent(
            renderer,
            [&](Event event) -> bool
            {
                if (event == Event::Character('q') ||
                    event == Event::Character('Q'))
                {
                    screen.Exit();
                    return true;
                }
                if (event == Event::Tab)
                {
                    left_active = !left_active;
                    return true;
                }
                auto* active_panel{left_active ? left_panel : right_panel};
                auto& cursor{left_active ? left_cursor : right_cursor};
                int entry_count{
                    static_cast<int>(active_panel->entryCount())};

                if (event == Event::ArrowDown)
                {
                    if (cursor < entry_count - 1)
                    {
                        ++cursor;
                    }
                    return true;
                }

                if (event == Event::ArrowUp)
                {
                    if (cursor > 0)
                    {
                        --cursor;
                    }
                    return true;
                }

                if (event == Event::Return)
                {
                    if (entry_count == 0)
                    {
                        return true;
                    }

                    auto entries{active_panel->entries()};
                    const auto& entry{
                        entries[static_cast<size_t>(cursor)]};
                    if (entry.type ==
                        wendfyr::domain::models::EntryType::DIRECTORY)
                    {
                        active_panel->navigateTo(entry.path);
                        cursor = 0;
                    }
                    return true;
                }

                if (event == Event::Backspace)
                {
                    active_panel->navigateUp();
                    cursor = 0;
                    return true;
                }

                if (event == Event::Character(' '))
                {
                    if (entry_count > 0)
                    {
                        active_panel->toggleSelection(
                            static_cast<size_t>(cursor));
                        if (cursor < entry_count - 1)
                        {
                            ++cursor;
                        }
                    }

                    return true;
                }

                if (event == Event::F5)
                {
                    auto selected{active_panel->selectedEntries()};
                    if (selected.empty() && entry_count > 0)
                    {
                        selected.push_back(
                            active_panel
                                ->entries()[static_cast<size_t>(cursor)]);
                    }

                    std::vector<std::filesystem::path> sources;
                    for (const auto& e : selected)
                    {
                        sources.push_back(e.path);
                    }

                    auto* dest_panel{left_active ? right_panel
                                                 : left_panel};

                    try
                    {
                        auto cmd{factory->createCopyCommand(
                            std::move(sources),
                            dest_panel->currentDirectory())};

                        executor->execute(std::move(cmd));
                        active_panel->deselectAll();
                    }
                    catch (const std::exception& e)
                    {
                        spdlog::error("Copy failed {}", e.what());
                    }
                    return true;
                }

                if (event == Event::F6)
                {
                    auto selected{active_panel->selectedEntries()};
                    if (selected.empty() && entry_count > 0)
                    {
                        selected.push_back(
                            active_panel
                                ->entries()[static_cast<size_t>(cursor)]);
                    }

                    std::vector<std::filesystem::path> sources;
                    for (const auto& e : selected)
                    {
                        sources.push_back(e.path);
                    }

                    auto* dest_panel{left_active ? right_panel
                                                 : left_panel};
                    try
                    {
                        auto cmd{factory->createMoveCommand(
                            std::move(sources),
                            dest_panel->currentDirectory())};
                        executor->execute(std::move(cmd));
                        active_panel->deselectAll();
                    }
                    catch (const std::exception& e)
                    {
                        spdlog::error("Move failed: {}", e.what());
                    }
                    return true;
                }

                if (event == Event::F7)
                {
                    auto new_dir{active_panel->currentDirectory() /
                                 "New Folder"};  // For now

                    try
                    {
                        auto cmd{factory->createCreateDirectoryCommand(
                            new_dir)};
                        executor->execute(std::move(cmd));
                    }
                    catch (const std::exception& e)
                    {
                        spdlog::error("Mkdir failed: {}", e.what());
                    }
                    return true;
                }

                if (event == Event::F8)
                {
                    auto selected{active_panel->selectedEntries()};
                    if (selected.empty() && entry_count > 0)
                    {
                        selected.push_back(
                            active_panel
                                ->entries()[static_cast<size_t>(cursor)]);
                    }

                    std::vector<std::filesystem::path> targets;
                    for (const auto& e : selected)
                    {
                        targets.push_back(e.path);
                    }

                    try
                    {
                        auto cmd{factory->createDeleteCommand(
                            std::move(targets))};
                        executor->execute(std::move(cmd));
                        active_panel->deselectAll();
                        cursor = 0;
                    }
                    catch (const std::exception& e)
                    {
                        spdlog::error("Delete faield: {}", e.what());
                    }

                    return true;
                }

                if (event == Event::Special("\x1A"))
                {
                    if (executor->canUndo())
                    {
                        try
                        {
                            executor->undo();
                        }
                        catch (const std::exception& e)
                        {
                            spdlog::error("Undo failed: {}", e.what());
                        }
                    }
                    return true;
                }

                if (event == Event::Special("\x19"))
                {
                    if (executor->canRedo())
                    {
                        try
                        {
                            executor->redo();
                        }
                        catch (const std::exception& e)
                        {
                            spdlog::error("Redo failed: {}", e.what());
                        }
                    }

                    return true;
                }

                return false;
            })};

        screen.Loop(component);

        return EXIT_SUCCESS;
    }
    catch (const std::exception& e)
    {
        spdlog::error("Fatail error: {}", e.what());
    }
    return EXIT_FAILURE;
}
