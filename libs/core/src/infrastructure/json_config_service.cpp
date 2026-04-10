#include "infrastructure/json_config_service.hpp"

#include <spdlog/spdlog.h>

#include <filesystem>
#include <fstream>
#include <optional>
#include <sstream>
#include <utility>

namespace wendfyr::infrastructure
{
    JsonConfigService::JsonConfigService(std::filesystem::path config_file_path)
        : _config_file_path{std::move(config_file_path)}
    {
    }

    void JsonConfigService::load()
    {
        if (!std::filesystem::exists(_config_file_path))
        {
            spdlog::info("Config file not found at {}, using defaults", _config_file_path.string());
            _data = nlohmann::json::object();
            return;
        }

        try
        {
            std::ifstream file(_config_file_path);
            if (!file.is_open())
            {
                spdlog::warn("Cannot oppen config file {}, using defaults",
                             _config_file_path.string());
                _data = nlohmann::json::object();
                return;
            }

            _data = nlohmann::json::parse(file);
            spdlog::info("Loaded config from {}", _config_file_path.string());
        }
        catch (const nlohmann::json::parse_error& e)
        {
            spdlog::warn("Malformed config file {}: {}. Using defaults", _config_file_path.string(),
                         e.what());

            _data = nlohmann::json::object();
        }
    }

    void JsonConfigService::save()
    {
        auto parent_dir = _config_file_path.parent_path();
        if (!parent_dir.empty() && !std::filesystem::exists(parent_dir))
        {
            std::filesystem::create_directories(parent_dir);
        }

        std::ofstream file(_config_file_path);
        if (!file.is_open())
        {
            spdlog::error("Cannot write config file {}", _config_file_path.string());
            return;
        }

        file << _data.dump(4);

        spdlog::info("Saved config to {}", _config_file_path.string());
    }

    std::optional<std::string> JsonConfigService::getString(const std::string& key) const
    {
        const auto* node = navigateTo(key);
        if (node == nullptr || !node->is_string())
        {
            return std::nullopt;
        }

        return node->get<std::string>();
    }

    void JsonConfigService::setString(const std::string& key, const std::string& value)
    {
        nlohmann::json& target = navigateOrCreate(key);
        target = value;
    }

    bool JsonConfigService::hasKey(const std::string& key) const
    {
        return navigateTo(key) != nullptr;
    }

    const nlohmann::json* JsonConfigService::navigateTo(const std::string& key) const
    {
        const nlohmann::json* current = &_data;
        std::istringstream stream(key);
        std::string segment;
        while (std::getline(stream, segment, '.'))
        {
            if (!current->is_object() || !current->contains(segment))
            {
                return nullptr;
            }
            current = &(*current)[segment];
        }

        return current;
    }

    nlohmann::json& JsonConfigService::navigateOrCreate(const std::string& key)
    {
        nlohmann::json* current = &_data;
        std::istringstream stream(key);
        std::string segment;

        std::vector<std::string> segments;
        while (std::getline(stream, segment, '.'))
        {
            segments.push_back(segment);
        }

        for (std::size_t i{0}; i + 1 < segments.size(); ++i)
        {
            if (!current->contains(segments[i]) || !(*current)[segments[i]].is_object())
            {
                (*current)[segments[i]] = nlohmann::json::object();
            }

            current = &(*current)[segments[i]];
        }

        return (*current)[segments.back()];
    }

};  // namespace wendfyr::infrastructure