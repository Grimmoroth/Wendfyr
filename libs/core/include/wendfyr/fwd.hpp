#ifndef WENDFYR_FWD_HPP
#define WENDFYR_FWD_HPP

#include <cstdint>

namespace wendfyr
{
    class ApplicationContext;
};

namespace wendfyr::ports::driving
{
    class ICommandExecutor;
    class ICommandFactory;
    class IPanelModel;
    enum class SortField : std::uint8_t;
    enum class SortOrder : std::uint8_t;
};  // namespace wendfyr::ports::driving

namespace wendfyr::ports::driven
{
    class IFilesystemService;
    class IConfigService;
};  // namespace wendfyr::ports::driven

namespace wendfyr::domain::commands
{
    class ICommand;
};

namespace wendfyr::domain::models
{
    struct FileEntry;
    enum class EntryType : std::uint8_t;
};  // namespace wendfyr::domain::models

namespace wendfyr::domain::events
{
    struct FilesCopiedEvent;
    struct FilesMovedEvent;
    struct FilesDeletedEvent;
    struct DirectoryChangedEvent;
    struct SelectionChangedEvent;
};  // namespace wendfyr::domain::events

namespace wendfyr::domain::errors
{
    class WendfyrError;
    class FileNotFoundException;
    class PermissionDeniedException;
    class DiskFullException;
    class InvalidPathException;
    class CrossDeviceMoveException;
    class FileAlreadyExistsException;
    class OperationCancelledException;
};  // namespace wendfyr::domain::errors

namespace wendfyr::services
{
    class EventBus;
    using SubscriptionId = std::size_t;
};  // namespace wendfyr::services

namespace wendfyr::services::logging
{
    enum class LogLevel : std::uint8_t;
    struct LogConfig;
};  // namespace wendfyr::services::logging

#endif
