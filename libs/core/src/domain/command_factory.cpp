#include "domain/command_factory.hpp"

#include <utility>

#include "domain/commands/copy_command.hpp"
#include "domain/commands/delete_command.hpp"
#include "domain/commands/move_command.hpp"

namespace wendfyr::domain
{
    CommandFactory::CommandFactory(ports::driven::IFilesystemService& fs,
                                   services::EventBus& event_bus)
        : _fs{fs}, _event_bus{event_bus}
    {
    }

    std::unique_ptr<domain::commands::ICommand> CommandFactory::createCopyCommand(
        std::vector<std::filesystem::path> sources, std::filesystem::path destination)
    {
        return std::make_unique<commands::CopyCommand>(std::move(sources), std::move(destination),
                                                       _fs, _event_bus);
    }

    std::unique_ptr<domain::commands::ICommand> CommandFactory::createMoveCommand(
        std::vector<std::filesystem::path> sources, std::filesystem::path destination)
    {
        return std::make_unique<commands::MoveCommand>(std::move(sources), std::move(destination),
                                                       _fs, _event_bus);
    }

    std::unique_ptr<domain::commands::ICommand> CommandFactory::createDeleteCommand(
        std::vector<std::filesystem::path> targets)
    {
        return std::make_unique<commands::DeleteCommand>(std::move(targets), _fs, _event_bus);
    }
};  // namespace wendfyr::domain