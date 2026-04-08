#ifndef WENDFYR_DOMAIN_COMMANDS_DELETE_COMMAND_HPP
#define WENDFYR_DOMAIN_COMMANDS_DELETE_COMMAND_HPP

#include "wendfyr/domain/commands/i_command.hpp"

#include <filesystem>
#include <string>
#include <utility>
#include <vector>

namespace wendfyr::ports::driven
{
    class IFilesystemService;
};

namespace wendfyr::services
{
    class EventBus;
}

namespace wendfyr::domain::commands
{

    class DeleteCommand final : public ICommand
    {
      public:
        DeleteCommand(std::vector<std::filesystem::path> targets,
                      ports::driven::IFilesystemService& fs, servies::EventBus& event_bus);
        DeleteCommand(DeleteCommand&) = delete;
        DeleteCommand& operator=(DeleteCommand&) = delete;
        DeleteCommand(DeleteCommand&&) noexcept = default;
        DeleteCommand& operator=(DeleteCommand&&) noexcept = default;
        ~DeleteCommand() override;

        void execute() override;
        void undo() override;
        [[nodiscard]] std::string description() const override;

      private:
        std::vector<std::filesystem::path> _targets;
        ports::driven::IFilesystemService& _fs;
        services::EventBus& _event_bus;

        std::filesystem::path _backup_directory;

        std::vector<std::pair<std::filesystem::path, std::filesystem::path>> _backup_records;
    };
};  // namespace wendfyr::domain::commands

#endif