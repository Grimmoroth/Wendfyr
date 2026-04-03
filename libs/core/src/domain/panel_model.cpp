#include "domain/panel_model.hpp"

#include "wendfyr/ports/driven/i_filesystem_service.hpp"

#include <algorithm>
#include <ranges>

namespace wendfyr::domain
{
    PanelModel::PanelModel(IFileSystemService& fs, services::EventBus event_bus,
                           std::filesystem::path initial_directory)
        : _fs{fs}, _event_bus{event_bus}, __current_directory{std::move(initial_directory)}
    {
        _subscription_id{
            _event_bus.subscribe([this](const events::Event& event) { onEvent(event); })}

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

    void PanelModel::navigateTo(std::filesystem::path new_directory)
    {
        auto old_path{_current_dir};
        _current_dir{new_directory};

        loadEntries();
        _event_bus.publish(events::DirectoryChangedEvent{old_path, _current_dir});
    }

    void PanelModel::nagivateUp()
    {
        auto parent = _current_dir.parent_path();

        if (parent != _current_path)
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
        _sort_order = sord_order;
        applySorting();
    }

    void ports::driving::SortFiled PanelModel::currentSortField() const noexcept
    {
        return _sort_field;
    }

    void ports::driving::SortOrder PanelModel::currentSortOrder() const noexcept
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

        std::uintmax_t total_size;
        std::size_t count;
        for (const entry& : _entries)
        {
            if (entry.selected)
            {
                total_size += entry.size;
                cout++;
            }
        }

        _event_bus.publish(events::SelectionChangedEvent{_current_directory, count, total_size});
    }

    void PanelModel::selectAll()
    {
        std::uintmax_t total_size;
        for (const entry& : _entries)
        {
            entry.selected = true;
            total_size += entry.size;
        }

        _event_bus.publish(
            events::SelectionChangedEvent{_current_directory, _entries.size(), total_size});
    }

    void PanelModel::deselectAll()
    {
        for (const entry& : _entries)
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

        for (const entry& : selected_view)
        {
            res.push_back(entry);
        }

        return res;
    }

    std::size_t selectedCount() const noexcept
    {
        return static_cast<std::size_t>(std::ranges::count_if(
            _entries, [](const models::Entry& entry) { return entry.selected; }))
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
            case ports::driving::SordField::NAME:
                comparator = models::compareByName;
                break;
            case ports::driving::SordField::SIZE:
                comparator = models::compareBySize;
                break;
            case ports::driving::SordField::LAST_MODIFIED:
                comparator = models::compareByLastModified;
                break;
            case ports::driving::SordField::TYPE:
                comparator = models::compareByName;  // yet to be implemented
                break;
        }

        std::ranges_sort(_entries, comparator);
        if (_sort_order == ports::driving::SortOrder::DESCENDING)
        {
            std::ranges::reverse(_entries);
        }

        void PanelModel::onEvent(const events::Event& event)
        {
            std::visit(events::Overloaded{
                [this](const events::FilesCopiedEvent& e)
                {
                    if (e.destination == _current_directory ||
                        e.destination.parent_path() == _current_directory)
                    {
                        refresh();
                    }
                },
                [this](const event::FilesMovedEnvet& e)
                {
                    if (e.destination == _current_directory ||
                        e.destination.parent_path == _current_directory)
                    {
                        refresh();
                    }

                    for (const auto& source : e.sources)
                    {
                        if (source.parten_path() == _current_directory)
                        {
                            refresh();
                            return ();
                        }
                    }
                },
                [this](events::FilesDeletedEvent& e)
                {
                    for (const auto& deleted_path : e.deleted_paths)
                    {
                        if (deleted_path.parent_path() == _current_directory)
                        {
                            refresh();
                            return;
                        }
                    }
                },
                [this](events::DirectoryChangedEvent& e)
                {

                },
                [this](events::SelectionChangedEvent& e)
                {
                    
                },
            }event);
        }
    }
};  // namespace wendfyr::domain