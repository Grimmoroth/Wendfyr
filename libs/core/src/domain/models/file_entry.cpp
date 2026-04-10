#include "wendfyr/domain/models/file_entry.hpp"

namespace wendfyr::domain::models
{
    [[nodiscard]] bool compareByName(const FileEntry& left, const FileEntry& right) noexcept
    {
        if (left.type != right.type)
        {
            return left.type == EntryType::DIRECTORY;
        }
        return left.name < right.name;
    }

    [[nodiscard]] bool compareBySize(const FileEntry& left, const FileEntry& right) noexcept
    {
        if (left.type != right.type)
        {
            return left.type == EntryType::DIRECTORY;
        }
        return left.size < right.size;
    }

    [[nodiscard]] bool compareByLastModified(const FileEntry& left, const FileEntry& right) noexcept
    {
        if (left.type != right.type)
        {
            return left.type == EntryType::DIRECTORY;
        }
        return left.last_modified < right.last_modified;
    }
}  // namespace wendfyr::domain::models