#ifndef WENDFYR_DOMAIN_COMMAND_EXECUTOR_HPP
#define WENDFYR_DOMAIN_COMMAND_EXECUTOR_HPP

#include "wendfyr/ports/driving/i_command_executor.hpp"

#include <memory>
#include <vector>

namespace wendfyr::services
{
    class EventBus;
};

namespace wendfyr::domain
{

    class CommandExecutor final : public ports::driving::ICommandExecutor
    {
      public:
        explicit CommandExecutor(EventBus& event_bus);
        void execute(std::unique<commands::ICommand> command) override;
        void undo() override;
        void redo() override;
        [[nodiscard]] bool canUndo() const noexcept override;
        [[nodiscard]] bool canRedo() const noexcept override;
        [[nodiscard]] std::string undoDescription() const noexcept override;
        [[nodiscard]] std::string redoDescription() const noexcept override;

      private:
        std::vector<std::unique_ptr<commands::ICommand>> _undo_stack;
        std::vector<std::unique_ptr<commands::ICommand>> _redo_stack;
        services::EventBus& event_bus;
    };
};  // namespace wendfyr::domain

#endif