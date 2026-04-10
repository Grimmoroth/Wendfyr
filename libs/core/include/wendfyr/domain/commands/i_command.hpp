#ifndef WENDFYR_DOMAIN_COMMANDS_I_COMMAND_HPP
#define WENDFYR_DOMAIN_COMMANDS_I_COMMAND_HPP

#include <string>

namespace wendfyr::domain::commands
{
    class ICommand
    {
      public:
        virtual ~ICommand();
        virtual void execute() = 0;
        virtual void undo() = 0;
        [[nodiscard]] virtual std::string description() const = 0;

      protected:
        ICommand() = default;
        ICommand(const ICommand&) = default;
        ICommand(ICommand&&) = default;
        ICommand& operator=(const ICommand&) = default;
        ICommand& operator=(ICommand&&) = default;
    };
};  // namespace wendfyr::domain::commands

#endif