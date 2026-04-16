#ifndef WENDFYR_DOMAIN_ERRORS_HPP
#define WENDFYR_DOMAIN_ERRORS_HPP

#include <filesystem>
#include <stdexcept>
#include <string>

namespace wendfyr::domain::errors
{
    class WendfyrError : public std::runtime_error
    {
      public:
        WendfyrError(const std::string& msg) : std::runtime_error(msg) {}
    };

    class FileNotFoundException : public WendfyrError
    {
      public:
        explicit FileNotFoundException(const std::filesystem::path& file_path)
            : WendfyrError("File not found: " + file_path.string()), _path{file_path}
        {
        }

        [[nodiscard]] const std::filesystem::path& path() const noexcept { return _path; }

      private:
        std::filesystem::path _path;
    };

    class PermissionDeniedException : public WendfyrError
    {
      public:
        explicit PermissionDeniedException(const std::filesystem::path& file_path)
            : WendfyrError("Permision Denied: " + file_path.string()), _path{file_path}
        {
        }

        const std::filesystem::path path() const noexcept { return _path; }

      private:
        std::filesystem::path _path;
    };

    class DiskFullException : public WendfyrError
    {
      public:
        DiskFullException(const std::filesystem::path file_path, std::uintmax_t required_bytes,
                          std::uintmax_t available_bytes)
            : WendfyrError("Disk full: need" + std::to_string(required_bytes) + " bytes, " +
                           std::to_string(available_bytes) + " available at " + file_path.string())
            , _path{file_path}
            , _required{required_bytes}
            , _available{available_bytes}
        {
        }

        [[nodiscard]] const std::filesystem::path path() const noexcept { return _path; }
        [[nodiscard]] std::uintmax_t requiredBytes() const noexcept { return _required; }
        [[nodiscard]] std::uintmax_t availableBytes() const noexcept { return _available; }

      private:
        std::filesystem::path _path;
        std::uintmax_t _required;
        std::uintmax_t _available;
    };

    class InvalidPathException : public WendfyrError
    {
      public:
        InvalidPathException(const std::filesystem::path& file_path, const std::string& reason = "")
            : WendfyrError("Invalid path: " + file_path.string() +
                           (reason.empty() ? "" : " (" + reason + ")"))
            , _path{file_path}
        {
        }

        [[nodiscard]] const std::filesystem::path path() const noexcept { return _path; }

      private:
        std::filesystem::path _path;
    };

    class CrossDeviceMoveException : public WendfyrError
    {
      public:
        CrossDeviceMoveException(const std::filesystem::path& source,
                                 const std::filesystem::path& dest)
            : WendfyrError("Cannot move across devices: " + source.string() + "->" + dest.string())
            , _source{source}
            , _dest{dest}
        {
        }

        [[nodiscard]] const std::filesystem::path source() const noexcept { return _source; }
        [[nodiscard]] const std::filesystem::path dest() const noexcept { return _dest; }

      private:
        std::filesystem::path _source;
        std::filesystem::path _dest;
    };

    class FileAlreadyExcistsException : public WendfyrError
    {
      public:
        FileAlreadyExcistsException(const std::filesystem::path& file_path)
            : WendfyrError("File already exists: " + file_path.string()), _path{file_path}
        {
        }

        [[nodiscard]] std::filesystem::path path() const noexcept { return _path; }

      private:
        std::filesystem::path _path;
    };

    class OperationCancelledException : public WendfyrError
    {
      public:
        OperationCancelledException() : WendfyrError("Operation cancelled by user.") {}
    };
};  // namespace wendfyr::domain::errors

#endif