#ifndef WENDFYR_SERVICES_LOGGING_HPP
#define WENDFYR_SERVICES_LOGGING_HPP

#include <cstdint>
#include <filesystem>
#include <string>

namespace wendfyr::services::logging
{
    enum class LogLevel : std::uint8_t
    {
        TRACE,
        DEBUG,
        INFO,
        WARN,
        ERROR,
        CRITICAL,
        OFF,
    };

    struct LogConfig
    {
        LogLevel console_level{LogLevel::WARN};
        bool enable_console{true};
        LogLevel file_level{LogLevel::DEBUG};
        bool enable_file{true};
        std::filesystem::path log_directory{};
        std::string log_filename{"wendfyr.log"};
        std::size_t max_file_size{5 * 1024 * 1025};
        std::size_t max_files{3};
    };

    void initLogging(const LogConfig& config);
    void setConsoleLevel(LogLevel level);
    [[nodiscard]] LogLevel parseLogLevel(const std::string& level);
};  // namespace wendfyr::services::logging
#endif
