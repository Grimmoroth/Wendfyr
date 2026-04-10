#include "wendfyr/bootstrap.hpp"
#include "wendfyr/domain/models/file_entry.hpp"

#include <spdlog/spdlog.h>

#include <cstdlib>
#include <filesystem>
#include <iostream>
#include <string>

int main()
{
    try
    {
        spdlog::set_level(spdlog::level::info);
        auto home = std::filesystem::current_path();
        auto app = wendfyr::createApplication(home);
        auto entries = app.left_panel->entries();
        std::cout << "Wendfyr - " << home.string() << '\n';
        std::cout << std::string(60, '-') << '\n';

        for (const auto& entry : entries)
        {
            std::string type_marker;
            if (entry.type == wendfyr::domain::models::EntryType::DIRECTORY)
            {
                type_marker = "[DIR]";
            }
            else if (entry.type == wendfyr::domain::models::EntryType::SYMLINK)
            {
                type_marker = "[LNK]";
            }
            else
            {
                type_marker = "       ";
            }
            std::cout << type_marker << entry.name;
            if (entry.type == wendfyr::domain::models::EntryType::FILE)
            {
                std::cout << " (" << entry.size << " bytes)";
            }

            std::cout << '\n';
        }

        std::cout << std::string(60, '-') << '\n';
        std::cout << entries.size() << " entries\n";

        return EXIT_SUCCESS;
    }
    catch (const std::exception& e)
    {
        spdlog::error("Fatal error: {}", e.what());
        return EXIT_FAILURE;
    }
}