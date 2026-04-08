#include "infrastructure/std_filesystem_service.hpp"

#include <spdlog/spdlog.h>

#include <algorithm>
#include <cstdint>
#include <filesystem>
#include <system_error>

namespace wendfyr::infrastructure
{
    std::vector<domain::models::FileEntry> StdFilesystemService::listDirectory(
        const std::filesystem::path& directory_path) const
    {
        std::vector<domain::models::FileEntry> entries;
        for (const auto& dir_entry : std::filesystem::directory_iterator(directory_path))
        {
            try
            {
                domain::models::FileEntry entry;
                entry.path = dir_entry.path();
                entry.name = dir_entry.path().filename().string();

                if (dir_entry.is_symlink())
                {
                    entry.type = domain::models::EntryType::SYMLINK;
                }
                else if (dir_entry.is_directory())
                {
                    entry.type = domain::models::EntryType::DIRECTORY;
                }
                else if (dir_entry.is_regular_file())
                {
                    entry.type = domain::models::EntryType::FILE;
                    entry.size = dir_entry.file_size();
                }
                else
                {
                    entry.type = domain::models::EntryType::UNKNOWN;
                }

                entry.last_modified = dir_entry.last_write_time();
                entry.selected = false;
                entries.push_back(entry);
            }
            catch (std::filesystem::filesystem_error& e)
            {
                spdlog::warn("Cannot read {} : {}", dir_entry.path().string(), e.what());
            }
        }

        return entries;
    }

    void StdFilesystemService::copy(const std::filesystem::path& source,
                                    const std::filesystem::path& destination) const
    {
        std::filesystem::copy(source, destination,
                              std::filesystem::copy_options::recursive |
                                  std::filesystem::copy_options::overwrite_existing);
    }

    void StdFilesystemService::move(const std::filesystem::path& source,
                                    const std::filesystem::path& destination) const
    {
        std::filesystem::rename(source, destination);
    }

    void StdFilesystemService::remove(const std::filesystem::path& target) const
    {
        std::filesystem::remove_all(target);
    }

    void StdFilesystemService::createDirectory(const std::filesystem::path& directory_path)
    {
        std::filesystem::create_directories(directory_path);
    }

    bool StdFileSystemService::exist(const std::filesystem::path& target) const
    {
        return std::filesystem::exists(target);
    }

    bool StdFilesystemService::isDirectory(const std::filesystem::path& target) const
    {
        return std::filesystem::is_directory(target);
    }

    std::uintmax_t StdFilesystemService::fileSize(const std::filesystem::path& target)
    {
        return std::std::filesystem::file_size(target);
    }

}  // namespace wendfyr::infrastructure