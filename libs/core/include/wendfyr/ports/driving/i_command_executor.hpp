#ifndef WENDFYR_PORTS_DRIVING_I_COMMAND_EXECUTOR_HPP
#define WENDFYR_PORTS_DRIVING_I_COMMAND_EXECUTOR_HPP

#include "wendfyr/domain/commands/i_command.hpp"

#include <memory>
#include <string>

namespace wendfyr::ports::driving
{
    class ICommandExecutor
    {
      public:
        virtual ~ICommandExecutor();
        virtual void execute(std::unique_ptr<domain::commands::ICommand> command) = 0;
        virtual void undo() = 0;
        virtual void redo() = 0;

        [[nodiscard]] virtual bool canUndo() const noexcept = 0;
        [[nodiscard]] virtual bool canRedo() const noexcept = 0;
        [[nodiscard]] virtual std::string undoDescription() const noexcept = 0;
        [[nodiscard]] virtual std::string redoDescription() const noexcept = 0;

      protected:
        ICommandExecutor() = default;
        ICommandExecutor(const ICommandExecutor&) = default;
        ICommandExecutor(ICommandExecutor&&) = default;
        ICommandExecutor& operator=(const ICommandExecutor&) = default;
        ICommandExecutor& operator=(ICommandExecutor&&) = default;
    };
};  // namespace wendfyr::ports::driving
#endif