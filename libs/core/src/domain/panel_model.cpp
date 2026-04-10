#include "domain/panel_model.hpp"

#include "wendfyr/domain/models/file_entry.hpp"
#include "wendfyr/ports/driven/i_filesystem_service.hpp"

#include <algorithm>
#include <ranges>

namespace wendfyr::domain
{
    PanelModel::PanelModel(ports::driven::IFilesystemService& fs, services::EventBus& event_bus,
                           std::filesystem::path initial_directory)
        : _fs{fs}, _event_bus{event_bus}, _current_directory{std::move(initial_directory)}
    {
        _subscription_id =
            _event_bus.subscribe([this](const events::Event& event) { onEvent(event); });

        loadEntries();
    }

    PanelModel::~PanelModel()
    {
        _event_bus.unsubscribe(_subscription_id);
    }

    std::filesystem::path PanelModel::currentDirectory() const
    {
        return _current_directory;
    }

    std::span<const models::FileEntry> PanelModel::entries() const
    {
        return _entries;
    }

    std::size_t PanelModel::entryCount() const noexcept
    {
        return _entries.size();
    }

    void PanelModel::navigateTo(const std::filesystem::path& new_directory)
    {
        auto old_path{_current_directory};
        _current_directory = new_directory;

        loadEntries();
        _event_bus.publish(events::DirectoryChangedEvent{old_path, _current_directory});
    }

    void PanelModel::navigateUp()
    {
        auto parent = _current_directory.parent_path();

        if (parent != _current_directory)
        {
            navigateTo(parent);
        }
    }

    void PanelModel::refresh()
    {
        loadEntries();
    }

    void PanelModel::sortBy(ports::driving::SortField sort_field,
                            ports::driving::SortOrder sort_order)
    {
        _sort_field = sort_field;
        _sort_order = sort_order;
        applySorting();
    }

    ports::driving::SortField PanelModel::currentSortField() const noexcept
    {
        return _sort_field;
    }

    ports::driving::SortOrder PanelModel::currentSortOrder() const noexcept
    {
        return _sort_order;
    }

    void PanelModel::toggleSelection(std::size_t index)
    {
        if (index >= _entries.size())
        {
            return;
        }

        _entries[index].selected = !_entries[index].selected;

        std::uintmax_t total_size{0};
        std::size_t count{0};
        for (const auto& entry : _entries)
        {
            if (entry.selected)
            {
                total_size += entry.size;
                count++;
            }
        }

        _event_bus.publish(events::SelectionChangedEvent{_current_directory, count, total_size});
    }

    void PanelModel::selectAll()
    {
        std::uintmax_t total_size{0};
        for (auto& entry : _entries)
        {
            entry.selected = true;
            total_size += entry.size;
        }

        _event_bus.publish(
            events::SelectionChangedEvent{_current_directory, _entries.size(), total_size});
    }

    void PanelModel::deselectAll()
    {
        for (auto& entry : _entries)
        {
            entry.selected = false;
        }

        _event_bus.publish(events::SelectionChangedEvent{_current_directory, 0, 0});
    }

    std::vector<models::FileEntry> PanelModel::selectedEntries() const
    {
        auto selected_view = _entries | std::views::filter([](const models::FileEntry& entry)
                                                           { return entry.selected; });

        std::vector<models::FileEntry> res;

        for (const auto& entry : selected_view)
        {
            res.push_back(entry);
        }

        return res;
    }

    std::size_t PanelModel::selectedCount() const noexcept
    {
        return static_cast<std::size_t>(std::ranges::count_if(
            _entries, [](const models::FileEntry& entry) { return entry.selected; }));
    }

    void PanelModel::loadEntries()
    {
        _entries = _fs.listDirectory(_current_directory);
        applySorting();
    }

    void PanelModel::applySorting()
    {
        auto comparator = models::compareByName;

        switch (_sort_field)
        {
            case ports::driving::SortField::NAME:
                comparator = models::compareByName;
                break;
            case ports::driving::SortField::SIZE:
                comparator = models::compareBySize;
                break;
            case ports::driving::SortField::LAST_MODIFIED:
                comparator = models::compareByLastModified;
                break;
            case ports::driving::SortField::TYPE:
                comparator = models::compareByName;  // yet to be implemented
                break;
        }

        std::ranges::sort(_entries, comparator);
        if (_sort_order == ports::driving::SortOrder::DESCENDING)
        {
            std::ranges::reverse(_entries);
        }
    }

    void PanelModel::onEvent(const events::Event& event)
    {
        std::visit(
            events::Overloaded{
                [this](const events::FilesCopiedEvent& e)
                {
                    // Refresh if the copy destination is our current directory
                    if (e.destination == _current_directory ||
                        e.destination.parent_path() == _current_directory)
                    {
                        refresh();
                    }
                },
                [this](const events::FilesMovedEvent& e)
                {
                    // Refresh if source or destination involves our directory
                    if (e.destination == _current_directory ||
                        e.destination.parent_path() == _current_directory)
                    {
                        refresh();
                    }
                    // Also refresh if files were moved OUT of our directory
                    for (const auto& source : e.sources)
                    {
                        if (source.parent_path() == _current_directory)
                        {
                            refresh();
                            return;
                        }
                    }
                },
                [this](const events::FilesDeletedEvent& e)
                {
                    // Refresh if any deleted file was in our directory
                    for (const auto& deleted_path : e.deleted_paths)
                    {
                        if (deleted_path.parent_path() == _current_directory)
                        {
                            refresh();
                            return;
                        }
                    }
                },
                [](const events::DirectoryChangedEvent&)
                {
                    // Another panel navigated — we don't care
                },
                [](const events::SelectionChangedEvent&)
                {
                    // Selection changed in some panel — we don't care
                },
            },
            event);
    }
};  // namespace wendfyr::domain