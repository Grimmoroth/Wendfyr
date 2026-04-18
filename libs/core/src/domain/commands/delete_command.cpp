#include "domain/commands/delete_command.hpp"

#include "wendfyr/ports/driven/i_filesystem_service.hpp"
#include "wendfyr/services/event_bus.hpp"

#include <spdlog/spdlog.h>

#include <chrono>
#include <filesystem>
#include <utility>

namespace wendfyr::domain::commands
{
    DeleteCommand::DeleteCommand(std::vector<std::filesystem::path> targets,
                                 ports::driven::IFilesystemService& fs,
                                 services::EventBus& event_bus)
        : _targets{std::move(targets)}, _fs{fs}, _event_bus{event_bus}
    {
    }

    DeleteCommand::~DeleteCommand()
    {
        try
        {
            if (!_backup_directory.empty() && _fs.exist(_backup_directory))
            {
                _fs.remove(_backup_directory);
                spdlog::debug("Cleaned up backup directory: {}", _backup_directory.string());
            }
        }
        catch (const std::exception& e)
        {
            spdlog::warn("Failed to clean up backup directory {}: {}", _backup_directory.string(),
                         e.what());
        }
    }

    void DeleteCommand::execute()
    {
        _backup_records.clear();
        _backup_directory =
            std::filesystem::temp_directory_path() /
            ("wf_backup_" +
             std::to_string(std::chrono::steady_clock::now().time_since_epoch().count()));

        _fs.createDirectory(_backup_directory);

        for (const auto& target : _targets)
        {
            auto backup_path = _backup_directory / target.filename();
            spdlog::info("Deleting {} (backed up to {})", target.string(), backup_path.string());

            _fs.move(target, backup_path);
            _backup_records.emplace_back(target, std::move(backup_path));
        }

        _event_bus.publish(events::FilesDeletedEvent{_targets});
        spdlog::info("Delete complete: {} file(s)", _targets.size());
    }

    void DeleteCommand::undo()
    {
        for (auto it{_backup_records.rbegin()}; it != _backup_records.rend(); ++it)
        {
            const auto& [original, backup] = *it;
            if (_fs.exist(backup))
            {
                spdlog::info("Undo delete: restoring {} -> {}", backup.string(), original.string());
                _fs.move(backup, original);
            }
            else
            {
                spdlog::warn("Undo delete: backup {} missing, cannot restore", backup.string());
            }
        }

        std::vector<std::filesystem::path> restored;
        for (const auto& [original, backup] : _backup_records)
        {
            restored.push_back(original);
        }

        _event_bus.publish(events::FilesCopiedEvent{.sources = restored,
                                                    .destination = _targets.front().parent_path()});
        spdlog::info("Undo delete complete: {} file(s) restored", _backup_records.size());
    }

    std::string DeleteCommand::description() const
    {
        if (_targets.size() == 1)
        {
            return "Delete : " + _targets.front().filename().string();
        }

        return "Delete " + std::to_string(_targets.size()) + " files";
    }
};  // namespace wendfyr::domain::commands