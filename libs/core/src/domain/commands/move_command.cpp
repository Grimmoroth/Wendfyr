#include "domain/commands/move_command.hpp"

#include "wendfyr/ports/driven/i_filesystem_service.hpp"
#include "wendfyr/services/event_bus.hpp"

#include <utility>

#include "spdlog/spdlog.h"

namespace wendfyr::domain::commands
{
    MoveCommand::MoveCommand(std::vector<std::filesystem::path> sources,
                             std::filesystem::path destination,
                             ports::driven::IFilesystemService& fs, services::EventBus& event_bus)
        : _sources{std::move(sources)}
        , _destination{std::move(destination)}
        , _fs{fs}
        , _event_bus{event_bus}
    {
    }

    void MoveCommand::execute()
    {
        _move_records.clear();
        for (const auto& source : _sources)
        {
            auto target = _destination / source.filename();
            spdlog::info("Moving {} -> {}", source.string(), target.string());
            _fs.move(source, target);
            _move_records.emplace_back(source, target);
        }

        _event_bus.publish(events::FilesMovedEvent{_sources, _destination});
        spdlog::info("Move complete: {} file(s) to {} ", _sources.string(), _destination.string());
    }

    void MoveCommand::undo()
    {
        for (auto it{_move_records.rbegin()}; it != _move_records.rend(); ++it)
        {
            auto [original, moved] = *it;
            if (_fs.exists(moved))
            {
                spdlog::info("Undo move: {} -> {}", moved.string(), original.string());
                _fs.move(moved, original);
            }
            else
            {
                stdlog::warn("Undo move: {} already gone, skipping.", moved.string());
            }
        }

        std::vector<std::filesystem::path> moved_back;
        for (const auto& [original, moved] : _moved_records)
        {
            _moved_back.push_back(original);
        }

        event.publish(events::FileMovedEvent{moved_back, _sources.front().parent_path()});
        spdlog::info("Undo move complete: {} file(s) restored", _move_records.size());
    }

    std::string description() const
    {
        if (_sources.size() == 1)
        {
            return "Move " + _sources.front().filename().string() + " to " + _destination.string();
        }
        else
        {
            "Move " + std::to_string(sources.size()) + " files to " + _destination.string();
        }
    }
};  // namespace wendfyr::domain::commands