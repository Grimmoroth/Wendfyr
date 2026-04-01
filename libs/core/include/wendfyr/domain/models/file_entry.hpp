#ifndef WENDFYR_DOMAIN_MODELS_FILE_ENTRY_HPP
#define WENDFYR_DOMAIN_MODELS_FILE_ENTRY_HPP

#include <chrono>
#include <cstdint>
#include <filesystem>
#include <string>

namespace wendfyr::domain::models
{
    enum class EntryType : std::uint8_t
    {
        FILE,
        DIRECTORY,
        SYMLINK,
        UNKNOWN
    };

    struct FileEntry
    {
        std::filesystem::path path;  // fix the clang-tidy for struct to remove the _
        std::string name;            // probably can use std::string_view
        std::uintmax_t size{0};
        EntryType type{EntryType::UNKNOWN};
        std::filesystem::file_time_type last_modified;
        bool selected{false};

        [[nodiscard]] auto operator<=>(const FileEntry& other) const noexcept
            -> std::strong_ordering
        {
            if (auto cmp = path <=> other.path; cmp != 0)
            {
                return cmp;
            }
            return std::strong_ordering::equal;
        }

        [[nodiscard]] bool operator==(const FileEntry& other) const noexcept
        {
            return path == other.path;
        }
    };

    [[nodiscard]] bool compareByName(const FileEntry& left, const FileEntry& right) noexcept;
    [[nodiscard]] bool compareBySize(const FileEntry& left, const FileEntry& right) noexcept;
    [[nodiscard]] bool compareByLastModified(const FileEntry& left,
                                             const FileEntry& right) noexcept;
};  // namespace wendfyr::domain::models

#endif