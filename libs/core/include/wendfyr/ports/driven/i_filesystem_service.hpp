#ifndef WENDFYR_PORTS_DRIVEN_I_FILESYSTEM_SERVICE_HPP
#define WENDFYR_PORTS_DRIVEN_I_FILESYSTEM_SERVICE_HPP

#include "wendfyr/domain/models/file_entry.hpp"

#include <filesystem>
#include <string>
#include <vector>

namespace wendfyr::ports::driven
{
    class IFileSystemService
    {
      public:
        virtual ~IFileSystemService() = default;
        [[nodiscard]] virtual std::vector<domain::models::FileEntry> listDirectory(
            const std::filesystem::path& dir_path) const = 0;

        virtual void copy(const std::filesystem::path& source,
                          const std::filesystem::path& dest) const = 0;

        virtual void move(const std::filesystem::path& source,
                          const std::filesystem::path& dest) const = 0;

        virtual void remove(const std::filesystem::path& parget) const = 0;

        virtual void createDirectory(const std::filesystem::path& dir_path) const = 0;

        [[nodiscard]] virtual bool exist(const std::filesystem::path& target) const = 0;
        [[nodiscard]] virtual bool isDirectory(const std::filesystem::path& parget) const = 0;
        [[nodiscard]] virtual std::uintmax_t fileSize(
            const std::filesystem::path& target) const = 0;

      protected:
        IFileSystemService() = default;
        IFileSystemService(const IFileSystemService&) = default;
        IFileSystemService(IFileSystemService&&) = default;
        IFileSystemService& operator=(const IFileSystemService&) = default;
        IFileSystemService& operator=(IFileSystemService&&) = default;
    };
};  // namespace wendfyr::ports::driven

#endif