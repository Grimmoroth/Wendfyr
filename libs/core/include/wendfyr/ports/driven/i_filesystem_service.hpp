#ifndef WENDFYR_PORTS_DRIVEN_I_FILESYSTEM_SERVICE_HPP
#define WENDFYR_PORTS_DRIVEN_I_FILESYSTEM_SERVICE_HPP

#include "wendfyr/domain/models/file_entry.hpp"

#include <filesystem>
#include <vector>

namespace wendfyr::ports::driven
{
    class IFilesystemService
    {
      public:
        virtual ~IFilesystemService();
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
        IFilesystemService() = default;
        IFilesystemService(const IFilesystemService&) = default;
        IFilesystemService(IFilesystemService&&) = default;
        IFilesystemService& operator=(const IFilesystemService&) = default;
        IFilesystemService& operator=(IFilesystemService&&) = default;
    };
};  // namespace wendfyr::ports::driven

#endif