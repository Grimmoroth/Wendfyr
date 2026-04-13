#include "wendfyr/domain/models/file_entry.hpp"
#include "wendfyr/ports/driven/i_filesystem_service.hpp"

#include <gmock/gmock.h>

namespace wendfyr::tests::mock
{

    class MockFilesystemService : public ports::driven::IFilesystemService
    {
      public:
        MOCK_METHOD(std::vector<domain::models::FileEntry>, listDirectory,
                    (const std::filesystem::path& directory_path), (const, override));

        MOCK_METHOD(void, copy,
                    (const std::filesystem::path& source, const std::filesystem::path& destination),
                    (const, override));

        MOCK_METHOD(void, move, (const std::filesystem::path& source, std::filesystem::path& dest),
                    (const, override));

        MOCK_METHOD(void, remove, (const std::filesystem::path& target), (const, override));
        MOCK_METHOD(void, createDirectory, (const std::filesystem::path target), (const, override));
        MOCK_METHOD(bool, exist, (const std::filesystem::path& target), (const, override));
        MOCK_METHOD(bool, isDirectory, (const std::filesystem::path& target), (const, override));
        MOCK_METHOD(std::uintmax_t, fileSize, (std::filesystem::path & target), (const, override));
    };

};  // namespace wendfyr::tests::mock