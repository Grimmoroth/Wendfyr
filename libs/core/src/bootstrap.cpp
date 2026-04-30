#include "wendfyr/bootstrap.hpp"

#include <spdlog/spdlog.h>

#include "domain/command_executor.hpp"
#include "domain/command_factory.hpp"
#include "domain/panel_model.hpp"
#include "infrastructure/json_config_service.hpp"
#include "infrastructure/std_filesystem_service.hpp"

namespace wendfyr
{
    ApplicationContext::ApplicationContext(
        std::shared_ptr<services::EventBus> event_bus,
        std::shared_ptr<ports::driven::IFilesystemService> fs,
        std::shared_ptr<ports::driven::IConfigService> config,
        std::unique_ptr<ports::driving::ICommandExecutor> executor,
        std::unique_ptr<ports::driving::ICommandFactory> factory,
        std::unique_ptr<ports::driving::IPanelModel> left,
        std::unique_ptr<ports::driving::IPanelModel> right)
        : _event_bus{std::move(event_bus)}
        , _fs{std::move(fs)}
        , config_service{std::move(config)}
        , command_executor{std::move(executor)}
        , command_factory{std::move(factory)}
        , left_panel{std::move(left)}
        , right_panel{std::move(right)}
    {
    }

    ApplicationContext createApplication(const std::filesystem::path& start_directory)
    {
        spdlog::info("Initializing Wendfyr...");

        // Infrastructure
        auto event_bus = std::make_shared<services::EventBus>();
        auto fs = std::make_shared<infrastructure::StdFilesystemService>();

        // Config
        auto config_path = start_directory / ".config" / "wendfyr" / "config.json";
        auto config = std::make_shared<infrastructure::JsonConfigService>(config_path);
        config->load();

        spdlog::info("Config loaded from {}", config_path.string());

        // Domain Services
        auto executor = std::make_unique<domain::CommandExecutor>(*event_bus);
        auto factory = std::make_unique<domain::CommandFactory>(*fs, *event_bus);

        // Panels
        auto left_panel = std::make_unique<domain::PanelModel>(*fs, *event_bus, start_directory);
        auto right_panel = std::make_unique<domain::PanelModel>(*fs, *event_bus, start_directory);

        spdlog::info("Wendfyr initialized. Start directory: {}", start_directory.string());

        return ApplicationContext{
            std::move(event_bus),   std::move(fs),      std::move(config),
            std::move(executor),    std::move(factory), std::move(left_panel),
            std::move(right_panel),
        };
    }
};  // namespace wendfyr
