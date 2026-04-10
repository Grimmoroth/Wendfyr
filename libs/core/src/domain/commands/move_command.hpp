#ifndef WENDFYR_DOMAIN_COMMANDS_MOVE_COMMAND_HPP
#define WENDFYR_DOMAIN_COMMANDS_MOVE_COMMAND_HPP
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
};

namespace wendfyr::domain::commands
{
    class MoveCommand final : public ICommand
    {
      public:
        MoveCommand(std::vector<std::filesystem::path> sources, std::filesystem::path destination,
                    ports::driven::IFilesystemService& fs, services::EventBus& event_bus);

        void execute() override;
        void undo() override;
        [[nodiscard]] std::string description() const override;

      private:
        std::vector<std::filesystem::path> _sources;
        std::filesystem::path _destination;
        std::vector<std::pair<std::filesystem::path, std::filesystem::path>> _moved_records;

        ports::driven::IFilesystemService& _fs;
        services::EventBus& _event_bus;
    };
};  // namespace wendfyr::domain::commands

#endif