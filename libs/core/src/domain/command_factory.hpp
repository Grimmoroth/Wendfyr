#ifndef WENDFYR_DOMAIN_COMMAND_FACTORY_HPP
#define WENDFYR_DOMAIN_COMMAND_FACTORY_HPP
#include "wendfyr/ports/driving/i_command_factory.hpp"

namespace wendfyr::ports::driven
{
    class IFilesystemService;
};

namespace wendfyr::services
{
    class EventBus;
}

namespace wendfyr::domain
{
    class CommandFactory final : public ports::driving::ICommandFactory
    {
      public:
        CommandFactory(ports::driven::IFilesystemService& fs, services::EventBus& event_bus);

        [[nodiscard]] std::unique_ptr<domain::commands::ICommand> createCopyCommand(
            std::vector<std::filesystem::path> sources, std::filesystem::path destination) override;

        [[nodiscard]] std::unique_ptr<domain::commands::ICommand> createMoveCommand(
            std::vector<std::filesystem::path> sources, std::filesystem::path destination) override;

        [[nodiscard]] std::unique_ptr<domain::commands::ICommand> createDeleteCommand(
            std::vector<std::filesystem::path> targets) override;

        [[nodiscard]] std::unique_ptr<domain::commands::ICommand> createCreateDirectoryCommand(
            std::filesystem::path dir_path) override;

      private:
        ports::driven::IFilesystemService& _fs;
        services::EventBus& _event_bus;
    };
};  // namespace wendfyr::domain

#endif