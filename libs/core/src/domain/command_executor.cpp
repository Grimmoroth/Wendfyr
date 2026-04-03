#include "domain/command_executor.hpp"

#include "wendfyr/services/event_bus.hpp"

#include <stdexcept>
#include <utility>

namespace wendfyr::domain
{
    CommandExecutor(services::EventBus& event_bus) : _event_bus{event_bus} {}

    void CommandExecutor::execute(std::unique_ptr<domain::commands::ICommand> command)
    {
        if (!command)
        {
            throw std::invalid_argument("Cannot execute a null command");
        }

        command->execute();
        _redo_stack.clear();

        _undo_stack.push_back(std::move(command));
    }

    void CommandExecutor::undo()
    {
        if (!canUndo())
        {
            std::runtime_error("Nothing to undo!");
        }

        auto command = std::move(_undo_stack.back());
        _undo_stack.pop_back();

        command->undo();
        _redo_stack.push_back(std::move(command));
    }

    void CommandExecutor::redo()
    {
        if (!canRedo())
        {
            throw std::runtime_error("Nothing to redo!");
        }

        auto command = std::move(_redo_stack.back());
        _redo_stack.pop_back();

        command->execute();
        _undo_stack.push_back(std::move(command));
    }

    bool CommandExecutor::canUndo() const noexcept
    {
        return !_undo_stack.empty();
    }

    bool CommandExecutor::canRedo() const noexcept
    {
        return !_redo_stack.empty();
    }

    std::string CommandExecutor::undoDescription() const
    {
        if (!canUndo())
        {
            return ""
        }

        return _undo_stack.back()->description();
    }

    std::string CommandExecutor::redoDescription() const
    {
        if (!canRedo())
        {
            return ""
        }

        return _redo_stack.back()->description();
    }
};  // namespace wendfyr::domain