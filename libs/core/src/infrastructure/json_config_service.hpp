#ifndef WENDFYR_INFRASTRUCTURE_JSON_CONFIG_SERVICE_HPP
#define WENDFYR_INFRASTRUCTURE_JSON_CONFIG_SERVICE_HPP

#include "wendfyr/ports/driven/i_config_service.hpp"

#include <nlohmann/json.hpp>

#include <filesystem>

namespace wendfyr::infrastructure
{
    class JsonConfigService final : public ports::driven::IConfigService
    {
      public:
        explicit JsonConfigService(std::filesystem::path config_file_path);
        [[nodiscard]] std::optional<std::string> getString(const std::string& key) const override;
        void setString(const std::string& key, const std::string& value) override;
        void load() override;
        void save() override;
        [[nodiscard]] bool hasKey(const std::string& key) const override;

      private:
        [[nodiscard]] const nlohmann::json* navigateTo(const std::string& key) const;
        [[nodiscard]] nlohmann::json& navigateOrCreate(const std::string& key);

        std::filesystem::path _config_file_path;
        nlohmann::json _data;
    };
}  // namespace wendfyr::infrastructure

#endif