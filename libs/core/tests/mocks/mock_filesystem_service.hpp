#ifndef WENDFYR_TESTS_MOCKS_MOCK_FILESYSTEM_SERVICE_HPP
#define WENDFYR_TESTS_MOCKS_MOCK_FILESYSTEM_SERVICE_HPP

#include "wendfyr/domain/models/file_entry.hpp"
#include "wendfyr/ports/driven/i_filesystem_service.hpp"

#include <gmock/gmock.h>

namespace wendfyr::tests::mocks
{

    class MockFilesystemService : public ports::driven::IFilesystemService
    {
      public:
        MOCK_METHOD(std::vector<domain::models::FileEntry>, listDirectory,
                    (const std::filesystem::path& directory_path), (const, override));

        MOCK_METHOD(void, copy,
                    (const std::filesystem::path& source, const std::filesystem::path& destination),
                    (const, override));

        MOCK_METHOD(void, move,
                    (const std::filesystem::path& source, const std::filesystem::path& dest),
                    (const, override));

        MOCK_METHOD(void, remove, (const std::filesystem::path& target), (const, override));
        MOCK_METHOD(void, createDirectory, (const std::filesystem::path& target),
                    (const, override));
        MOCK_METHOD(bool, exist, (const std::filesystem::path& target), (const, override));
        MOCK_METHOD(bool, isDirectory, (const std::filesystem::path& target), (const, override));
        MOCK_METHOD(std::uintmax_t, fileSize, (const std::filesystem::path& target),
                    (const, override));
    };

};  // namespace wendfyr::tests::mocks

#endif