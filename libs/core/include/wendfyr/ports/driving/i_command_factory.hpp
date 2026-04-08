#ifndef WENDFYR_PORTS_DRIVING_I_COMMAND_FACTORY_HPP
#define WENDFYR_PORTS_DRIVING_I_COMMAND_FACTORY_HPP

#include "wendfyr/domain/commands/i_command.hpp"

#include <filesystem>
#include <memory>
#include <vector>

namespace wendfyr::ports::driving
{
    class ICommandFactory
    {
      public:
        virtual ~ICommandFactory();

        [[nodiscard]] virtual std::unique_ptr<domain::commands::ICommand> createCopyCommand(
            std::vector<std::filesystem::path> sources, std::filesystem::path destination) = 0;
        [[nodiscard]] virtual std::unique_ptr<domain::commands::ICommand> createMoveCommand(
            std::vector<std::filesystem::path> sources, std::filesystem::path destination) = 0;

        [[nodiscard]] virtual std::unique_ptr<domain::commands::ICommand> createDeleteCommand(
            std::vector<std::filesystem::path> targets) = 0;

      protected:
        ICommandFactory() = default;
        ICommandFactory(const ICommandFactory&) = default;
        ICommandFactory(ICommandFactory&&) = default;
        ICommandFactory& operator=(const ICommandFactory&) = default;
        ICommandFactory& operator=(ICommandFactory&&) = default;
    };
};  // namespace wendfyr::ports::driving
#endif