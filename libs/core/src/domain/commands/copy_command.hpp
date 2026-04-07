#ifndef WENDFYR_DOMAIN_COMMANDS_COPY_COMMAND_HPP
#define WENDFYR_DOMAIN_COMMANDS_COPY_COMMAND_HPP

#include "wendfyr/domain/commands/i_command.hpp"
#include "wendfyr/domain/events/event.hpp"

#include <filesystem>
#include <string>
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
    class CopyCommand final : public ICommand
    {
      public:
        CopyCommand(std::vector<std::filesystem::path> sources, std::filesystem::path destination,
                    ports::driven::IFilesystemService& fs, services::EventBus& event_bus);

        void execute() override;
        void undo() override;
        [[nodiscard]] std::string undoDescription() const override;

      private:
        std::vector<std::filesystem::path> _sources;
        std::filesystem::path _destination;
        std::vector<std::filesystem::path> _created_files;
        ports::driven::IFilesystemService& _fs;
        services::EventBus& _event_bus
    };
}  // namespace wendfyr::domain::commands

#endif