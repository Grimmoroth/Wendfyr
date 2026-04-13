#ifndef WENDFYR_TESTS_MOCKS_MOCK_CONFIG_SERVICE_HPP
#define WENDFYR_TESTS_MOCKS_MOCK_CONFIG_SERVICE_HPP

#include "wendfyr/ports/driven/i_config_service.hpp"

#include <gmock/gmock.h>

#include <optional>

namespace wendfyr::tests::mocks
{
    class MockConfigService : public ports::driven::IConfigService
    {
      public:
        MOCK_METHOD(std::optional<std::string>, getString, (const std::string& key),
                    (const, override));

        MOCK_METHOD(void, setString, (const std::string& key, const std::string& value),
                    (override));

        MOCK_METHOD(void, load, (), (override));
        MOCK_METHOD(void, save, (), (override));

        MOCK_METHOD(bool, hasKey, (const std::string& key), (const, override));
    }
};  // namespace wendfyr::tests::mocks

#endif