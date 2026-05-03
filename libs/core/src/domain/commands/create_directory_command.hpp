#ifndef WENDFYR_DOMAIN_COMMANDS_CREATE_DIRECTORY_COMMAND_CPP
#define WENDFYR_DOMAIN_COMMANDS_CREATE_DIRECTORY_COMMAND_CPP
#include "wendfyr/domain/commands/i_command.hpp"

#include <filesystem>
#include <string>

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
    class CreateDirectoryCommand final : public commands::ICommand
    {
      public:
        CreateDirectoryCommand(std::filesystem::path dir_path,
                               ports::driven::IFilesystemService& fs,
                               services::EventBus& event_bus);
        void execute() override;
        void undo() override;
        [[nodiscard]] std::string description() const override;

      private:
        std::filesystem::path _dir_path;

        ports::driven::IFilesystemService& _fs;
        services::EventBus& _event_bus;

        bool _created{false};
    };

};  // namespace wendfyr::domain::commands

#endif
