#include "panel_view.hpp"

#include "wendfyr/domain/models/file_entry.hpp"

#include <ftxui/dom/elements.hpp>

using namespace ftxui;

namespace wendfyr::tui
{
    PanelView::PanelView(ports::driving::IPanelModel& model)
        : _model{model}
    {
    }

    Element PanelView::render(bool is_active) const
    {
        auto entries{_model.entries()};
        auto header{
            hbox({text(" " + _model.currentDirectory().string() + " ") |
                  bold})};

        auto col_headers{
            hbox({text(" Name") | size(WIDTH, EQUAL, 30),
                  text(" Size") | size(WIDTH, EQUAL, 8) | align_right}) |
            dim};

        Elements rows;
        for (int i{0}; i < static_cast<int>(entries.size()); ++i)
        {
            const auto& entry{entries[static_cast<size_t>(i)]};
            std::string prefix{"  "};
            if (entry.type == domain::models::EntryType::DIRECTORY)
            {
                prefix = "📁";
            }
            else if (entry.type == domain::models::EntryType::SYMLINK)
            {
                prefix = "🔗";
            }

            std::string size_str{};
            if (entry.type == domain::models::EntryType::FILE)
            {
                size_str = formatSize(entry.size);
            }

            auto row{hbox(
                {text(prefix + " " + entry.name) | size(WIDTH, EQUAL, 30),
                 text(size_str) | size(WIDTH, EQUAL, 8) | align_right})};

            bool is_cursor{i == _cursor};
            if (is_cursor && is_active)
            {
                row = row | inverted | focus;
            }
            else if (is_cursor)
            {
                row = row | inverted;
            }

            if (entry.selected)
            {
                row = row | color(Color::Yellow);
            }

            else if (entry.type == domain::models::EntryType::DIRECTORY &&
                     !is_cursor)
            {
                row = row | color(Color::Cyan);
            }
            rows.push_back(row);
        }
        auto file_list{
            vbox({std::move(rows) | vscroll_indicator | yframe})};

        std::string footer_text{
            " " + std::to_string(_model.entries().size()) + " entries"};

        if (auto selected_count{_model.selectedCount()};
            selected_count > 0)
        {
            footer_text +=
                " | " + std::to_string(selected_count) + " selected";
        }
        auto footer{hbox(footer_text) | dim};

        auto border_style{is_active ? borderHeavy : border};

        return vbox({
                   header,
                   separator(),
                   col_headers,
                   separator(),
                   file_list | flex,
                   separator(),
                   footer,
               }) |
               border_style | flex;
    }

    void PanelView::cursorDown()
    {
        if (_cursor < static_cast<int>(entryCount()) - 1)
        {
            ++_cursor;
        }
    }

    void PanelView::cursorUp()
    {
        if (_cursor > 0)
        {
            --_cursor;
        }
    }

    void PanelView::cursorToTop()
    {
        _cursor = 0;
    }

    void PanelView::cursorToBottom()
    {
        int count{static_cast<int>(entryCount())};
        _cursor = count > 0 ? count - 1 : 0;
    }

    void PanelView::resetCursor()
    {
        _cursor = 0;
    }

    int PanelView::cursor() const noexcept
    {
        return _cursor;
    }

    void PanelView::navigateTo(const std::filesystem::path& directory)
    {
        _model.navigateTo(directory);
        resetCursor();
    }

    void PanelView::navigateUp()
    {
        _model.navigateUp();
        resetCursor();
    }

    void PanelView::toogleSelection()
    {
        if (entryCount() > 0)
        {
            _model.toggleSelection(static_cast<size_t>(_cursor));
            cursorDown();
        }
    }

    void PanelView::selectAll()
    {
        _model.selectAll();
    }
    void PanelView::deselectAll()
    {
        _model.deselectAll();
    }

    std::filesystem::path PanelView::currentDirectory() const
    {
        return _model.currentDirectory();
    }

    std::size_t PanelView::entryCount() const noexcept
    {
        return _model.entryCount();
    }

    std::vector<domain::models::FileEntry> PanelView::selectedEntries()
        const
    {
        return _model.selectedEntries();
    }

    const domain::models::FileEntry* PanelView::currentEntry() const
    {
        auto entries{_model.entries()};
        if (_cursor >= 0 && _cursor < static_cast<int>(entries.size()) - 1)
        {
            return &entries[static_cast<size_t>(_cursor)];
        }
        return nullptr;
    }

    std::string PanelView::formatSize(std::uintmax_t bytes)
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

};  // namespace wendfyr::tui
