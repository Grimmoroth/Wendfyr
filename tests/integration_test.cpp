#include "wendfyr/bootstrap.hpp"
#include "wendfyr/domain/errors.hpp"
#include "wendfyr/domain/events/event.hpp"
#include "wendfyr/domain/models/file_entry.hpp"
#include "wendfyr/ports/driving/i_panel_model.hpp"

#include <gtest/gtest.h>

#include <algorithm>
#include <chrono>
#include <filesystem>
#include <fstream>
#include <iterator>
#include <string>
#include <vector>

namespace wendfyr::tests
{
    class IntegrationTest : public ::testing::Test
    {
      protected:
        std::filesystem::path test_dir;
        std::filesystem::path source_dir;
        std::filesystem::path dest_dir;

        void SetUp() override
        {
            test_dir =
                std::filesystem::temp_directory_path() /
                ("wf_test_" +
                 std::to_string(std::chrono::steady_clock::now().time_since_epoch().count()));

            source_dir = test_dir / "source";
            dest_dir = test_dir / "dest";

            std::filesystem::create_directories(source_dir);
            std::filesystem::create_directories(dest_dir);

            createFile(source_dir / "hello.txt", "Hello World");
            createFile(source_dir / "data.csv", "name,age\nAlice,30\nBob,25");
            createFile(source_dir / "empty.txt", "");

            std::filesystem::create_directories(source_dir / "subdir");
            createFile(source_dir / "subdir" / "nested.txt", "Nested content");
        }

        void TearDown() override { std::filesystem::remove_all(test_dir); }

        static void createFile(const std::filesystem::path& path, const std::string& content)
        {
            std::ofstream file(path);
            file << content;
        }

        static std::string readFile(const std::filesystem::path& path)
        {
            std::ifstream file(path);
            return {std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>()};
        }

        static bool containsEntry(std::span<const domain::models::FileEntry> entries,
                                  const std::string& name)
        {
            return std::ranges::any_of(
                entries, [&](const domain::models::FileEntry& e) { return e.name == name; });
        }
    };

    TEST_F(IntegrationTest, BootstrapCreateWorkingApplication)
    {
        auto ctx{createApplication(source_dir)};
        ASSERT_NE(ctx.command_executor, nullptr);
        ASSERT_NE(ctx.command_factory, nullptr);
        ASSERT_NE(ctx.left_panel, nullptr);
        ASSERT_NE(ctx.right_panel, nullptr);
        ASSERT_NE(ctx.config_service, nullptr);

        EXPECT_EQ(ctx.left_panel->currentDirectory(), source_dir);
        EXPECT_EQ(ctx.right_panel->currentDirectory(), source_dir);
    }

    TEST_F(IntegrationTest, PanelListsRealDirectory)
    {
        auto ctx{createApplication(source_dir)};

        auto entries{ctx.left_panel->entries()};

        EXPECT_TRUE(containsEntry(entries, "hello.txt"));
        EXPECT_TRUE(containsEntry(entries, "data.csv"));
        EXPECT_TRUE(containsEntry(entries, "empty.txt"));
        EXPECT_TRUE(containsEntry(entries, "subdir"));

        EXPECT_GE(ctx.left_panel->entryCount(), 4);
    }

    TEST_F(IntegrationTest, PanelNavigatesToSubdirectory)
    {
        auto ctx{createApplication(source_dir)};
        ctx.left_panel->navigateTo(source_dir / "subdir");
        EXPECT_EQ(ctx.left_panel->currentDirectory(), source_dir / "subdir");

        auto entries{ctx.left_panel->entries()};
        EXPECT_TRUE(containsEntry(entries, "nested.txt"));
        EXPECT_GE(ctx.left_panel->entryCount(), 1);
    }

    TEST_F(IntegrationTest, PanelNavigatesUp)
    {
        auto ctx{createApplication(source_dir / "subdir")};
        ctx.left_panel->navigateUp();

        EXPECT_EQ(ctx.left_panel->currentDirectory(), source_dir);
    }

    TEST_F(IntegrationTest, CopyFileEndToEnd)
    {
        auto ctx{createApplication(source_dir)};

        auto cmd{ctx.command_factory->createCopyCommand({source_dir / "hello.txt"}, dest_dir)};
        ctx.command_executor->execute(std::move(cmd));

        // Copied?
        ASSERT_TRUE(std::filesystem::exists(dest_dir / "hello.txt"));
        EXPECT_EQ(readFile(dest_dir / "hello.txt"), "Hello World");

        // source is still there?
        EXPECT_TRUE(std::filesystem::exists(source_dir / "hello.txt"));

        // Undo works?
        ASSERT_TRUE(ctx.command_executor->canUndo());
        ctx.command_executor->undo();

        // Removed?
        EXPECT_FALSE(std::filesystem::exists(dest_dir / "hello.txt"));

        EXPECT_TRUE(std::filesystem::exists(source_dir / "hello.txt"));
    }

    TEST_F(IntegrationTest, CopyMultipleFilesEndToEnd)
    {
        auto ctx{createApplication(source_dir)};
        auto cmd{ctx.command_factory->createCopyCommand(
            {source_dir / "hello.txt", source_dir / "data.csv"}, dest_dir)};
        ctx.command_executor->execute(std::move(cmd));

        EXPECT_TRUE(std::filesystem::exists(dest_dir / "hello.txt"));
        EXPECT_TRUE(std::filesystem::exists(dest_dir / "data.csv"));

        ctx.command_executor->undo();

        EXPECT_FALSE(std::filesystem::exists(dest_dir / "hello.txt"));
        EXPECT_FALSE(std::filesystem::exists(dest_dir / "data.csv"));

        EXPECT_TRUE(std::filesystem::exists(source_dir / "data.csv"));
        EXPECT_TRUE(std::filesystem::exists(source_dir / "hello.txt"));
        EXPECT_EQ(readFile(source_dir / "hello.txt"), "Hello World");
    }

    TEST_F(IntegrationTest, MoveFileEndToEnd)
    {
        auto ctx{createApplication(source_dir)};
        auto cmd{ctx.command_factory->createMoveCommand({source_dir / "hello.txt"}, dest_dir)};

        ctx.command_executor->execute(std::move(cmd));

        // Moved?
        ASSERT_TRUE(std::filesystem::exists(dest_dir / "hello.txt"));
        EXPECT_EQ(readFile(dest_dir / "hello.txt"), "Hello World");

        EXPECT_FALSE(std::filesystem::exists(source_dir / "hello.txt"));

        ASSERT_TRUE(ctx.command_executor->canUndo());
        ctx.command_executor->undo();

        // Moved back to the source?
        EXPECT_TRUE(std::filesystem::exists(source_dir / "hello.txt"));
        EXPECT_EQ(readFile(source_dir / "hello.txt"), "Hello World");

        EXPECT_FALSE(std::filesystem::exists(dest_dir / "hello.txt"));
    }

    TEST_F(IntegrationTest, DeleteFileEndToEnd)
    {
        auto ctx{createApplication(source_dir)};
        auto cmd{ctx.command_factory->createDeleteCommand({source_dir / "hello.txt"})};
        ctx.command_executor->execute(std::move(cmd));

        EXPECT_FALSE(std::filesystem::exists(source_dir / "hello.txt"));

        EXPECT_TRUE(ctx.command_executor->canUndo());
        ctx.command_executor->undo();

        ASSERT_TRUE(std::filesystem::exists(source_dir / "hello.txt"));
        EXPECT_EQ(readFile(source_dir / "hello.txt"), "Hello World");
    }

    TEST_F(IntegrationTest, CreateDirectoryEndToEnd)
    {
        auto ctx{createApplication(source_dir)};
        auto new_dir{source_dir / "new_dir"};
        auto cmd{ctx.command_factory->createCreateDirectoryCommand(new_dir)};
        ctx.command_executor->execute(std::move(cmd));

        EXPECT_TRUE(std::filesystem::exists(new_dir));
        EXPECT_TRUE(std::filesystem::is_directory(new_dir));

        EXPECT_TRUE(ctx.command_executor->canUndo());
        ctx.command_executor->undo();

        EXPECT_FALSE(std::filesystem::exists(new_dir));
    }

    TEST_F(IntegrationTest, CreateDirectoryUndoSkipsNonEmpty)
    {
        auto ctx{createApplication(source_dir)};
        auto new_dir{source_dir / "will_be_filled"};
        auto cmd{ctx.command_factory->createCreateDirectoryCommand(new_dir)};
        ctx.command_executor->execute(std::move(cmd));

        EXPECT_TRUE(std::filesystem::exists(new_dir));

        createFile(new_dir / "user_file.txt", "Important data");

        ctx.command_executor->undo();
        EXPECT_TRUE(std::filesystem::exists(new_dir));
        EXPECT_TRUE(std::filesystem::exists(new_dir / "user_file.txt"));
    }

    TEST_F(IntegrationTest, UndoRedoChain)
    {
        auto ctx{createApplication(source_dir)};
        auto cmd{ctx.command_factory->createCopyCommand({source_dir / "hello.txt"}, dest_dir)};

        // Copied
        ctx.command_executor->execute(std::move(cmd));
        EXPECT_TRUE(std::filesystem::exists(dest_dir / "hello.txt"));

        // Undo
        ctx.command_executor->undo();
        EXPECT_FALSE(std::filesystem::exists(dest_dir / "hello.txt"));

        // Redo
        ctx.command_executor->redo();
        EXPECT_TRUE(std::filesystem::exists(dest_dir / "hello.txt"));

        // Undo again
        ctx.command_executor->undo();
        EXPECT_FALSE(std::filesystem::exists(dest_dir / "hello.txt"));
    }

    TEST_F(IntegrationTest, PanelRefreshesAfterCopy)
    {
        auto ctx{createApplication(source_dir)};
        ctx.left_panel->navigateTo(dest_dir);
        EXPECT_EQ(ctx.left_panel->entryCount(), 0);

        auto cmd{ctx.command_factory->createCopyCommand({source_dir / "hello.txt"}, dest_dir)};
        ctx.command_executor->execute(std::move(cmd));

        EXPECT_TRUE(containsEntry(ctx.left_panel->entries(), "hello.txt"));
        EXPECT_EQ(ctx.left_panel->entryCount(), 1);
    }

    TEST_F(IntegrationTest, CopyNonExistentFileThrows)
    {
        auto ctx{createApplication(source_dir)};
        auto cmd{
            ctx.command_factory->createCopyCommand({source_dir / "doesn't_exist.txt"}, dest_dir)};

        EXPECT_THROW(ctx.command_executor->execute(std::move(cmd)),
                     domain::errors::FileNotFoundException);

        EXPECT_FALSE(ctx.command_executor->canUndo());
    }

    TEST_F(IntegrationTest, PanelSortBySize)
    {
        auto ctx{createApplication(source_dir)};
        ctx.left_panel->sortBy(ports::driving::SortField::SIZE,
                               ports::driving::SortOrder::ASCENDING);

        std::vector<std::uintmax_t> file_sizes{};
        for (const auto& file : ctx.left_panel->entries())
        {
            if (file.type == domain::models::EntryType::FILE)
            {
                file_sizes.push_back(file.size);
            }
        }

        EXPECT_TRUE(std::ranges::is_sorted(file_sizes));
    }

}  // namespace wendfyr::tests