#include "domain/commands/create_directory_command.hpp"

#include "wendfyr/domain/events/event.hpp"
#include "wendfyr/ports/driven/i_filesystem_service.hpp"
#include "wendfyr/services/event_bus.hpp"

#include <spdlog/spdlog.h>

#include <utility>

namespace wendfyr::domain::commands
{

    CreateDirectoryCommand::CreateDirectoryCommand(std::filesystem::path& dir_path,
                                                   ports::driven::IFilesystemService& fs,
                                                   services::EventBus& event_bus)
        : _dir_path{std::move(dir_path)}, _fs{fs}, _event_bus{event_bus}
    {
    }

    void CreateDirectoryCommand::execute()
    {
        if (_fs.exist(_dir_path))
        {
            spdlog::info("Directory already exist: {}", _dir_path.string());
            _created = false;
            return;
        }

        _fs.createDirectory(_dir_path);
        _created = true;

        _event_bus.publish(
            events::FilesCopiedEvent{.sources{_dir_path}, .destination{_dir_path.parent_path()}});

        spdlog::info("Created directory {} ", _dir_path.string());
    }

    void CreateDirectoryCommand::undo()
    {
        if (!_created)
        {
            spdlog::info("Undo create directory: skipping (may not created by Wendfyr)");
            return;
        }

        if (!_fs.exist(_dir_path))
        {
            spd::warn("Undo create directory: {} already gone", _dir_path.string());
            _created = false;
            return;
        }

        if (!_fs.listDirectory(_dir_path).empty())
        {
            spd::warn("Undo create directory: {} is not empty, skipping", _dir_path.string());
            return;
        }

        _fs.remove(_dir_path);
        _created = false;

        _event_bus.publish(events::FilesDeletedEvent{.deleted_paths{_dir_path}});

        spdlog::info("Undo create directory: removed {}", _dir_path.string());
    }

    std::string CreateDirectoryCommand::description() const
    {
        return "Create directory: " + _dir_path.filename().string();
    }
};  // namespace wendfyr::domain::commands