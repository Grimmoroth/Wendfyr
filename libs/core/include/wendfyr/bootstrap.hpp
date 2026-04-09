#ifndef WENDFYR_BOOTSTRAP_HPP
#define WENDFYR_BOOTSTRAP_HPP

#include "wendfyr/ports/driven/i_config_service.hpp"
#include "wendfyr/ports/driven/i_filesystem_service.hpp"
#include "wendfyr/ports/driving/i_command_executor.hpp"
#include "wendfyr/ports/driving/i_command_factory.hpp"
#include "wendfyr/ports/driving/i_panel_model.hpp"
#include "wendfyr/services/event_bus.hpp"

#include <filesystem>
#include <memory>

namespace wendfyr
{

    class ApplicationContext
    {
        ApplicationContext() = default;
        std::shared_ptr<std::services::EventBus> _event_bus;
        std::shared_ptr<ports::driven::IFilesystemService> _fs;
        friend ApplicationContext createApplication(const std::filesystem::path& start_directory);

        ApplicationContext(std::shared_ptr<std::services::EventBus> event_bus,
                           std::shared_ptr<ports::driven::IFilesystemService> fs,
                           std::shared_ptr<ports::driven::IConfigService> config,
                           std::unique_ptr<ports::driving::ICommandExecutor> executor,
                           std::unique_ptr<ports::driving::ICommandFactory> factory,
                           std::unique_ptr<ports::driving::IPanelModel> left,
                           std::unique_ptr<ports::driving::IPanelModel> right);

      public:
        ~ApplicationContext() = default;
        ApplicationContext(const ApplicationContext&) = delete;
        ApplicationContext& operator=(const ApplicationContext&) = delete;
        ApplicationContext(ApplicationContext&&) noexcept = default;
        ApplicationContext& operator=(ApplicationContext&&) noexcept = default;

        std::shared_ptr<ports::driven::IConfigService> config_service;
        std::unique_ptr<ports::driving::ICommandExecutor> command_executor;
        std::unique_ptr<ports::driving::ICommandFactory> command_factory;
        std::unique_ptr<ports::driving::IPanelModel> left_panel;
        std::unique_ptr<ports::driving::IPanelModel> right_panel;
    };

    [[nodiscard]] ApplicationContext createApplication(
        const std::filesystem::path& start_directory);
};  // namespace wendfyr

#endif