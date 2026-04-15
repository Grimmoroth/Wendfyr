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
            : WendfyrError("File not found" + file_path.string()), _path{file_path}
        {
        }

        [[nodiscard]] const std::filesystem::path& path() const noexcept { return _path; }

      private:
        std::filesystem::path _path;
    };
};  // namespace wendfyr::domain::errors

#endif