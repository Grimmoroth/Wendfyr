#include "wendfyr/bootstrap.hpp"
#include "wendfyr/domain/errors.hpp"
#include "wendfyr/domain/models/file_entry.hpp"
#include "wendfyr/ports/driving/i_panel_model.hpp"
#include "wendfyr/services/logging.hpp"

#include <CLI/CLI.hpp>
#include <spdlog/common.h>
#include <spdlog/spdlog.h>

#include <cstdlib>
#include <filesystem>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

namespace
{
    std::string formatSize(std::uintmax_t bytes)
    {
        constexpr std::uintmax_t KB{1024};
        constexpr std::uintmax_t MB{KB * 1024};
        constexpr std::uintmax_t GB{MB * 1024};
        constexpr std::uintmax_t TB{GB * 1024};

        std::ostringstream out;
        out << std::fixed << std::setprecision(1);

        if (bytes >= TB)
        {
            out << static_cast<double>(bytes) / static_cast<double>(TB) << " TB";
        }
        else if (bytes >= GB)
        {
            out << static_cast<double>(bytes) / static_cast<double>(GB) << " GB";
        }
        else if (bytes >= MB)
        {
            out << static_cast<double>(bytes) / static_cast<double>(MB) << " MB";
        }
        else if (bytes >= KB)
        {
            out << static_cast<double>(bytes) / static_cast<double>(KB) << " KB";
        }
        else
        {
            out << bytes << " B";
        }

        return out.str();
    }

    void printListing(const wendfyr::ports::driving::IPanelModel& panel)
    {
        std::cout << "\n " << panel.currentDirectory().string() << "\n ";
        std::cout << "  " << std ::string(58, '-') << "\n";

        for (const auto& entry : panel.entries())
        {
            std::string type_marker{};
            switch (entry.type)
            {
                case wendfyr::domain::models::EntryType::DIRECTORY:
                    type_marker = "[DIR ]";
                    break;
                case wendfyr::domain::models::EntryType::SYMLINK:
                    type_marker = "[LNK ]";
                    break;
                case wendfyr::domain::models::EntryType::FILE:
                    type_marker = "       ";
                    break;
                case wendfyr::domain::models::EntryType::UNKNOWN:
                    type_marker = "  [?]  ";
                    break;
            }

            std::cout << "  " << type_marker << std::left << std::setw(35) << entry.name;

            if (entry.type == wendfyr::domain::models::EntryType::FILE)
            {
                std::cout << std::right << std::setw(10) << formatSize(entry.size);
            }

            std::cout << '\n';
        }

        std::cout << "  " << std::string(58, '-') << '\n';
        std::cout << "  " << panel.entryCount() << " entries.\n\n";
    }
}  // namespace

int main(int argc, char** argv)
{
    try
    {
        CLI::App app{"Wendfyr - command-line file manager"};
        app.require_subcommand(1);
        bool verbose{false};
        bool debug{false};

        app.add_flag("-v,--verbose", verbose, "Enable verbose logging");
        app.add_flag("--debug", debug, "Enable debug logging");
        std::string list_dir{};
        std::string sort_field{"name"};
        bool sort_descending{false};

        // LIST
        auto* list_cmd{app.add_subcommand("list", "List directory contents")};
        list_cmd->add_option("directory", list_dir, "Directory to list")->default_val(".");
        list_cmd->add_option("--sort,-s", sort_field, "Sort by: name, size, date")
            ->default_val("name");
        list_cmd->add_flag("--desc,-d", sort_descending, "Sort descending");
        list_cmd->alias("ls");

        // COPY
        std::vector<std::string> copy_args;
        auto* copy_cmd{app.add_subcommand("copy", "Copy files to destination")};
        copy_cmd->add_option("paths", copy_args, "Source files and destination")
            ->required()
            ->expected(-2);
        copy_cmd->alias("cp");

        // MOVE
        std::vector<std::string> move_args;
        auto* move_cmd{app.add_subcommand("move", "Move files to destination")};
        move_cmd->add_option("paths", move_args, "Source files and destination")
            ->required()
            ->expected(-2);
        move_cmd->alias("mv");

        // Delete
        std::vector<std::string> delete_targets;
        auto* delete_cmd{app.add_subcommand("delete", "Delete files")};
        delete_cmd->add_option("targets", delete_targets, "Files to delete")
            ->required()
            ->expected(-1);
        delete_cmd->alias("rm");
        delete_cmd->alias("del");

        std::string mkdir_path{};
        auto* mkdir_cmd{app.add_subcommand("mkdir", "Create Directory")};
        mkdir_cmd->alias("md");
        mkdir_cmd->add_option("path", mkdir_path, "Directory to create")->required();

        CLI11_PARSE(app, argc, argv);

        auto log_level{wendfyr::services::logging::LogLevel::WARN};
        if (debug)
        {
            log_level = wendfyr::services::logging::LogLevel::DEBUG;
        }
        else if (verbose)
        {
            log_level = wendfyr::services::logging::LogLevel::INFO;
        }

        wendfyr::services::logging::initLogging({.console_level = log_level,
                                                 .enable_file = true,
                                                 .log_directory = ""});  // TODO: fix this <--

        auto home{std::filesystem::current_path()};
        auto ctx{wendfyr::createApplication(home)};

        if (list_cmd->parsed())
        {
            auto dir{std::filesystem::absolute(list_dir)};
            if (dir != ctx.left_panel->currentDirectory())
            {
                // todo: First set orderering then navigate?
                ctx.left_panel->navigateTo(dir);

                auto field{wendfyr::ports::driving::SortField::NAME};
                if (sort_field == "size")
                {
                    field = wendfyr::ports::driving::SortField::SIZE;
                }
                if (sort_field == "date")
                {
                    field = wendfyr::ports::driving::SortField::LAST_MODIFIED;
                }

                auto order{sort_descending ? wendfyr::ports::driving::SortOrder::DESCENDING
                                           : wendfyr::ports::driving::SortOrder::ASCENDING};

                ctx.left_panel->sortBy(field, order);
                printListing(*ctx.left_panel);
            }
        }
        else if (copy_cmd->parsed())
        {
            auto dest{std::filesystem::absolute(copy_args.back())};
            std::vector<std::filesystem::path> sources{};
            sources.reserve(copy_args.size() - 1);

            for (std::size_t i{0}; i + 1 < copy_args.size(); ++i)
            {
                sources.push_back(std::filesystem::absolute(copy_args[i]));
            }

            std::cout << " Copying " << sources.size() << " file(s) to" << dest.string() << "...\n";

            auto cmd{ctx.command_factory->createCopyCommand(std::move(sources), dest)};
            ctx.command_executor->execute(std::move(cmd));

            std::cout << "Done.\n";
        }
        else if (move_cmd->parsed())
        {
            auto dest{std::filesystem::absolute(move_args.back())};
            std::vector<std::filesystem::path> sources{};
            sources.reserve(move_args.size() - 1);

            for (std::size_t i{0}; i + 1 < move_args.size(); ++i)
            {
                sources.push_back(std::filesystem::absolute(move_args[i]));
            }
            std::cout << " Moving " << sources.size() << " file(s) to" << dest.string() << "...\n";
            auto cmd{ctx.command_factory->createMoveCommand(std::move(sources), dest)};
            ctx.command_executor->execute(std::move(cmd));

            std::cout << "Done.\n";
        }
        else if (delete_cmd->parsed())
        {
            std::vector<std::filesystem::path> targets{};
            targets.reserve(delete_targets.size());
            for (const auto& target : delete_targets)
            {
                targets.push_back(std::filesystem::absolute(target));
            }

            std::cout << " Deleting " << targets.size() << " file(s)...\n";
            auto cmd{ctx.command_factory->createDeleteCommand(std::move(targets))};
            ctx.command_executor->execute(std::move(cmd));

            std::cout << " Done.\n";
        }
        else if (mkdir_cmd->parsed())
        {
            auto dir{std::filesystem::absolute(mkdir_path)};
            std::cout << "  Creating Directory  " << dir.string() << "...\n";
            auto cmd{ctx.command_factory->createCreateDirectoryCommand(dir)};
            ctx.command_executor->execute(std::move(cmd));

            std::cout << "  Done.\n";
        }
        return EXIT_SUCCESS;
    }
    catch (const wendfyr::domain::errors::FileNotFoundException& e)
    {
        std::cerr << "  File not found:  " << e.path().string() << '\n';
        return EXIT_FAILURE;
    }
    catch (const wendfyr::domain::errors::PermissionDeniedException& e)
    {
        std::cerr << "  Perminsion denied:  " << e.path().string() << '\n';
        return EXIT_FAILURE;
    }
    catch (const wendfyr::domain::errors::DiskFullException& e)
    {
        std::cerr << "  Disk full at:  " << e.path().string() << '\n';
        return EXIT_FAILURE;
    }
    catch (const wendfyr::domain::errors::FileAlreadyExcistsException& e)
    {
        std::cerr << "  File already exists: " << e.path().string() << '\n';
        return EXIT_FAILURE;
    }
    catch (const wendfyr::domain::errors::OperationCancelledException& e)
    {
        return EXIT_FAILURE;
    }
    catch (const wendfyr::domain::errors::WendfyrError& e)
    {
        std::cerr << "  Error:  " << e.what() << '\n';
    }
    catch (const std::exception& e)
    {
        std::cerr << "  Unexpected error:  " << e.what() << '\n';
        return EXIT_FAILURE;
    }
}
