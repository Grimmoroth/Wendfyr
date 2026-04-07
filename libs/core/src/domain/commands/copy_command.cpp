#include "domain/commands/copy_command.hpp"

#include "wendfyr/ports/driven/i_filesystem_service.hpp"
#include "wendfyr/services/event_bus.hpp"

#include <spdlog/spdlog.h>

namespace wendfyr::domain::commands
{
    CopyCommand::CopyCommand(std::vector<std::filesystem::path> sources,
                             std::filesystem::path destination, IFilesystemService& fs,
                             EventBus& event_bus)
        : _sources{std::move(sources)}
        , _destination{std::move(destination)}
        , _fs{fs}
        , _event_bus{event_bus}
    {
    }

    void CopyCommand::execute()
    {
        _created_fieles.clear();

        for (const auto& source : _sources)
        {
            auto target = _destination / source.filename();
            spdlog::info("Copying {} - > {}", source.string(), target.string());

            _fs.copy(source, target);
            _created_files.push_back(target);
        }

        _event_bus.publish(events::FIlesCopiedEvent{_sources, _destination});

        spdlog::info("Copying complete {} file(s) to {}", _sources.size(), _destination.string());
    }

    void CopyCommand::undo()
    {
        for (auto it{_created_files.rbegin()}; it != _created_files.rend(); ++it)
        {
            if (_fs.exists(*it))
            {
                spdlog::info("Undo copy: deleting {}", it->string());
                _fs.remove(*it);
            }
            else
            {
                spdlog::warn("Undo copy: {} already gone, skipping", it->string())
            }
        }

        _event_bus.publish(events::FilesDeletedEvent{_created_files});
        spdlog::info("Undo copy complete: {} file(s) removed" _created_files.size());
    }

    std::string CopyCommand::description() const
    {
        if (_soruces.size() == 1)
        {
            "Copy " + _sources.front().filename().string() + " to " + _destination..string();
        }

        return "Copy " + std::to_string(_sources.size()) + " files to " + _destination.string();
    }
};  // namespace wendfyr::domain::commands