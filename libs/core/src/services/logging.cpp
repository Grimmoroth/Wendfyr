#include "wendfyr/services/logging.hpp"

#include <spdlog/sinks/rotating_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>

#include <filesystem>
#include <iostream>
#include <memory>
#include <string>

namespace wendfyr::services::logging
{

    namespace
    {

        spdlog::level::level_enum toSpdlogLevel(LogLevel level)
        {
            switch (level)
            {
                case LogLevel::TRACE:
                    return spdlog::level::trace;
                case LogLevel::DEBUG:
                    return spdlog::level::debug;
                case LogLevel::INFO:
                    return spdlog::level::info;
                case LogLevel::WARN:
                    return spdlog::level::warn;
                case LogLevel::ERROR:
                    return spdlog::level::err;
                case LogLevel::CRITICAL:
                    return spdlog::level::critical;
                case LogLevel::OFF:
                    return spdlog::level::off;
            }
            return spdlog::level::warn;
        }
        std::shared_ptr<spdlog::sinks::stderr_color_sink_mt> g_console_sink;
    };  // namespace

    void initLogging(const LogConfig& config)
    {
        std::vector<spdlog::sink_ptr> sinks;
        if (config.enable_console)
        {
            g_console_sink = std::make_shared<spdlog::sinks::stderr_color_sink_mt>();
            g_console_sink->set_level(toSpdlogLevel(config.console_level));
            // [10:30:45] [W] Cross-device move: using copy + delete
            g_console_sink->set_pattern("[T] [%^%L%$] %v");

            sinks.push_back(g_console_sink);
        }

        if (config.enable_file)
        {
            try
            {
                auto log_dir{config.log_directory};
                if (log_dir.empty())
                {
                    const char* home{std::getenv("HOME")};
                    std::cerr << home << '\n';
                    if (home != nullptr)
                    {
                        log_dir = std::filesystem::path(home) / ".local" / "share" / "wendfyr";
                    }
                    else
                    {
                        log_dir = std::filesystem::temp_directory_path() / "wendfyr";
                    }
                }
                std::filesystem::create_directory(log_dir);
                auto file_path{log_dir / config.log_filename};

                auto file_sink{std::make_shared<spdlog::sinks::rotating_file_sink_mt>(
                    file_path.string(), config.max_file_size, config.max_files)};

                file_sink->set_level(toSpdlogLevel(config.file_level));

                // [2026-04-19 10:30:45.123] [debug] [bootstrap.cpp:42] Message
                file_sink->set_pattern("[%Y-%m-%d %T.%e] [%l] [%s:%#] %v");

                sinks.push_back(file_sink);
            }
            catch (const std::exception& err)
            {
                std::cerr << "Warning: could not create log file: " << err.what() << '\n';
            }
        }

        auto logger{std::make_shared<spdlog::logger>("Wendfyr", sinks.begin(), sinks.end())};
        logger->set_level(spdlog::level::trace);
        logger->flush_on(spdlog::level::warn);

        spdlog::set_default_logger(std::move(logger));
    }

    void setConsoleLevel(LogLevel level)
    {
        if (g_console_sink)
        {
            g_console_sink->set_level(toSpdlogLevel(level));
        }
    }

    LogLevel parseLogLevel(const std::string& level)
    {
        if (level == "trace")
        {
            return LogLevel::TRACE;
        }
        if (level == "debug")
        {
            return LogLevel::DEBUG;
        }
        if (level == "info")
        {
            return LogLevel::INFO;
        }
        if (level == "warn")
        {
            return LogLevel::WARN;
        }
        if (level == "error")
        {
            return LogLevel::ERROR;
        }
        if (level == "critical")
        {
            return LogLevel::CRITICAL;
        }
        if (level == "OFF")
        {
            return LogLevel::OFF;
        }

        return LogLevel::WARN;
    }
};  // namespace wendfyr::services::logging
