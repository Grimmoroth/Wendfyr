#ifndef WENDFYR_INFRASTRUCTURE_STD_FILESYSTEM_SERVICE_HPP
#define WENDFYR_INFRASTRUCTURE_STD_FILESYSTEM_SERVICE_HPP
#include "wendfyr/domain/models/file_entry.hpp"
#include "wendfyr/ports/driven/i_filesystem_service.hpp"

namespace wendfyr::infrastructure
{
    class StdFilesystemService final : public ports::driven::IFilesystemService
    {
      public:
        StdFilesystemService() = default;

        [[nodiscard]] std::vector<domain::models::FileEntry> listDirectory(
            const std::filesystem::path& path) const override;

        void copy(const std::filesystem::path& sources,
                  const std::filesystem::path& destination) const override;

        void move(const std::filesystem::path& sources,
                  const std::filesystem::path& destination) const override;

        void remove(const std::filesystem::path& target) const override;

        void createDirectory(const std::filesystem::path& directory_path) const override;

        [[nodiscard]] bool exist(const std::filesystem::path& target) const override;
        [[nodiscard]] bool isDirectory(const std::filesystem::path& target) const override;
        [[nodiscard]] std::uintmax_t fileSize(const std::filesystem::path& target) const override;
    };
};  // namespace wendfyr::infrastructure
#endif