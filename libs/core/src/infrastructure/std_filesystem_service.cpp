#include "infrastructure/std_filesystem_service.hpp"

#include "wendfyr/domain/errors.hpp"

#include <spdlog/spdlog.h>

#include <cstdint>
#include <filesystem>

namespace wendfyr::infrastructure
{

    namespace
    {
        [[noreturn]] void translateError(const std::error_code& code,
                                         const std::filesystem::path& ctx_path)
        {
            if (code == std::errc::no_such_file_or_directory)
            {
                throw domain::errors::FileNotFoundException(ctx_path);
            }
            if (code == std::errc::permission_denied)
            {
                throw domain::errors::PermissionDeniedException(ctx_path);
            }
            if (code == std::errc::no_space_on_device)
            {
                throw domain::errors::DiskFullException(ctx_path, 0, 0);
            }

            throw domain::errors::WendfyrError("Filesystem error at: " + ctx_path.string() + ": " +
                                               code.message());
        }
    };  // namespace

    std::vector<domain::models::FileEntry> StdFilesystemService::listDirectory(
        const std::filesystem::path& directory_path) const
    {
        if (!std::filesystem::exists(directory_path))
        {
            throw domain::errors::FileNotFoundException(directory_path);
        }

        if (!std::filesystem::is_directory(directory_path))
        {
            throw domain::errors::InvalidPathException(directory_path, "not a directory");
        }

        std::vector<domain::models::FileEntry> entries;
        try
        {
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
        }
        catch (const std::filesystem::filesystem_error& e)
        {
            translateError(e.code(), directory_path);
        }

        return entries;
    }

    void StdFilesystemService::copy(const std::filesystem::path& source,
                                    const std::filesystem::path& destination) const
    {
        try
        {
            std::filesystem::copy(source, destination,
                                  std::filesystem::copy_options::recursive |
                                      std::filesystem::copy_options::overwrite_existing);
        }
        catch (const std::filesystem::filesystem_error& e)
        {
            translateError(e.code(), destination);
        }
    }

    void StdFilesystemService::move(const std::filesystem::path& source,
                                    const std::filesystem::path& destination) const
    {
        try
        {
            std::filesystem::rename(source, destination);
            return;
        }
        catch (const std::filesystem::filesystem_error& e)
        {
            if (e.code() != std::errc::cross_device_link)
            {
                translateError(e.code(), source);
            }
        }

        spdlog::info("Cross device move: {} -> {}", source.string(), destination.string());
        copy(source, destination);
        remove(source);
    }

    void StdFilesystemService::remove(const std::filesystem::path& target) const
    {
        try
        {
            std::filesystem::remove_all(target);
        }
        catch (std::filesystem::filesystem_error& e)
        {
            translateError(e.code(), target);
        }
    }

    void StdFilesystemService::createDirectory(const std::filesystem::path& directory_path) const
    {
        try
        {
            std::filesystem::create_directories(directory_path);
        }
        catch (std::filesystem::filesystem_error& e)
        {
            translateError(e.code(), directory_path);
        }
    }

    bool StdFilesystemService::exist(const std::filesystem::path& target) const
    {
        return std::filesystem::exists(target);
    }

    bool StdFilesystemService::isDirectory(const std::filesystem::path& target) const
    {
        return std::filesystem::is_directory(target);
    }

    std::uintmax_t StdFilesystemService::fileSize(const std::filesystem::path& target) const
    {
        return std::filesystem::file_size(target);
    }

}  // namespace wendfyr::infrastructure