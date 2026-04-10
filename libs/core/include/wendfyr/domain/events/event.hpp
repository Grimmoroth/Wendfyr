#ifndef WENDFYR_DOMAIN_EVENTS_EVENT_HPP
#define WENDFYR_DOMAIN_EVENTS_EVENT_HPP

#include <cstddef>
#include <filesystem>
#include <variant>
#include <vector>

namespace wendfyr::domain::events
{
    struct FilesCopiedEvent
    {
        std::vector<std::filesystem::path> sources;
        std::filesystem::path destination;
    };

    struct FilesMovedEvent
    {
        std::vector<std::filesystem::path> sources;
        std::filesystem::path destination;
    };

    struct FilesDeletedEvent
    {
        std::vector<std::filesystem::path> deleted_paths;
    };

    struct DirectoryChangedEvent
    {
        std::filesystem::path old_path;
        std::filesystem::path new_path;
    };

    struct SelectionChangedEvent
    {
        std::filesystem::path directory;
        std::size_t selected_count;
        std::uintmax_t total_size;
    };

    using Event = std::variant<FilesCopiedEvent, FilesMovedEvent, FilesDeletedEvent,
                               DirectoryChangedEvent, SelectionChangedEvent>;

    template <typename... Ts>
    struct Overloaded : Ts...
    {
        using Ts::operator()...;
    };

    template <typename... Ts>
    Overloaded(Ts...) -> Overloaded<Ts...>;

};  // namespace wendfyr::domain::events

#endif