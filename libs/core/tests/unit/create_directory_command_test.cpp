#include "domain/commands/create_directory_command.hpp"

#include "wendfyr/domain/models/file_entry.hpp"
#include "wendfyr/services/event_bus.hpp"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <memory>

#include "gmock/gmock.h"
#include "mocks/mock_filesystem_service.hpp"

namespace wendfyr::tests
{
    class CreateDirectoryCommandTest : public ::testing::Test
    {
      protected:
        testing::NiceMock<mocks::MockFilesystemService> mock_fs;
        services::EventBus event_bus;

        std::filesystem::path new_dir = "/home/user/new_folder";

        void SetUp() override
        {
            ON_CALL(mock_fs, exist(testing::_)).WillByDefault(testing::Return(false));
        }

        std::unique_ptr<domain::commands::CreateDirectoryCommand> makeCommand()
        {
            return std::make_unique<domain::commands::CreateDirectoryCommand>(new_dir, mock_fs,
                                                                              event_bus);
        }
    };

    TEST_F(CreateDirectoryCommandTest, ExecuteCreateDirectory)
    {
        ON_CALL(mock_fs, exist(new_dir)).WillByDefault(testing::Return(false));

        EXPECT_CALL(mock_fs, createDirectory(new_dir)).Times(1);

        auto cmd{makeCommand()};
        cmd->execute();
    }

    TEST_F(CreateDirectoryCommandTest, ExecuteSkipsIfAlreadyExists)
    {
        ON_CALL(mock_fs, exist(new_dir)).WillByDefault(testing::Return(true));

        EXPECT_CALL(mock_fs, createDirectory(testing::_)).Times(0);

        auto cmd{makeCommand()};
        cmd->execute();
    }

    TEST_F(CreateDirectoryCommandTest, ExecutePublishesEvent)
    {
        ON_CALL(mock_fs, exist(new_dir)).WillByDefault(testing::Return(false));
        EXPECT_CALL(mock_fs, createDirectory(testing::_)).Times(1);

        bool event_received{false};
        event_bus.subscribe(
            [&](const domain::events::Event& event)
            {
                std::visit(
                    domain::events::Overloaded{
                        [&](const domain::events::FilesCopiedEvent&) { event_received = true; },
                        [](const auto&) {},
                    },
                    event);
            });

        auto cmd{makeCommand()};
        cmd->execute();

        EXPECT_TRUE(event_received);
    }

    TEST_F(CreateDirectoryCommandTest, ExecuteNoEventIfAlreadyExist)
    {
        ON_CALL(mock_fs, exist(new_dir)).WillByDefault(testing::Return(true));

        bool event_received{false};

        event_bus.subscribe(
            [&](const domain::events::Event& event)
            {
                std::visit(
                    domain::events::Overloaded{
                        [&](const domain::events::FilesCopiedEvent&) { event_received = true; },
                        [](const auto&) {},
                    },
                    event);
            });

        auto cmd{makeCommand()};
        cmd->execute();

        EXPECT_FALSE(event_received);
    }

    TEST_F(CreateDirectoryCommandTest, UndoRemovesCreatedDirectory)
    {
        ON_CALL(mock_fs, exist(new_dir)).WillByDefault(testing::Return(false));
        EXPECT_CALL(mock_fs, createDirectory(new_dir)).Times(1);

        auto cmd{makeCommand()};
        cmd->execute();

        testing::Mock::VerifyAndClearExpectations(&mock_fs);

        ON_CALL(mock_fs, exist(new_dir)).WillByDefault(testing::Return(true));
        ON_CALL(mock_fs, listDirectory(new_dir))
            .WillByDefault(testing::Return(std::vector<domain::models::FileEntry>{}));

        EXPECT_CALL(mock_fs, remove(new_dir)).Times(1);

        cmd->undo();
    }

    TEST_F(CreateDirectoryCommandTest, UndoSkipsIfNotCreatedByWendfyr)
    {
        ON_CALL(mock_fs, exist(new_dir)).WillByDefault(testing::Return(true));

        auto cmd{makeCommand()};
        cmd->execute();

        EXPECT_CALL(mock_fs, remove(testing::_)).Times(0);

        cmd->undo();
    }

    TEST_F(CreateDirectoryCommandTest, UndoSkipsIfDirectoryNotEmpty)
    {
        ON_CALL(mock_fs, exist(new_dir)).WillByDefault(testing::Return(false));

        EXPECT_CALL(mock_fs, createDirectory(new_dir)).Times(1);

        auto cmd{makeCommand()};
        cmd->execute();

        testing::Mock::VerifyAndClearExpectations(&mock_fs);

        ON_CALL(mock_fs, exist(new_dir)).WillByDefault(testing::Return(true));

        domain::models::FileEntry child;
        child.path = new_dir / "some_file.txt";
        child.name = "some_file";
        child.type = domain::models::EntryType::FILE;

        ON_CALL(mock_fs, listDirectory(new_dir))
            .WillByDefault(testing::Return(std::vector<domain::models::FileEntry>{child}));

        EXPECT_CALL(mock_fs, remove(testing::_)).Times(0);

        cmd->undo();
    }

    TEST_F(CreateDirectoryCommandTest, UndoSkipsIfAlreadyGone)
    {
        ON_CALL(mock_fs, exist(new_dir)).WillByDefault(testing::Return(false));
        EXPECT_CALL(mock_fs, createDirectory(new_dir)).Times(1);

        auto cmd{makeCommand()};
        cmd->execute();

        testing::Mock::VerifyAndClearExpectations(&mock_fs);
        ON_CALL(mock_fs, exist(new_dir)).WillByDefault(testing::Return(false));
        EXPECT_CALL(mock_fs, remove(testing::_)).Times(0);

        cmd->undo();
    }

    TEST_F(CreateDirectoryCommandTest, UndoPublishesEvent)
    {
        ON_CALL(mock_fs, exist(new_dir)).WillByDefault(testing::Return(false));
        EXPECT_CALL(mock_fs, createDirectory(new_dir)).Times(1);

        auto cmd{makeCommand()};
        cmd->execute();

        testing::Mock::VerifyAndClearExpectations(&mock_fs);

        ON_CALL(mock_fs, exist(new_dir)).WillByDefault(testing::Return(true));
        ON_CALL(mock_fs, listDirectory(new_dir))
            .WillByDefault(testing::Return(std::vector<domain::models::FileEntry>{}));

        bool event_received{false};

        event_bus.subscribe(
            [&](const domain::events::Event& event)
            {
                std::visit(
                    domain::events::Overloaded{
                        [&](const domain::events::FilesDeletedEvent&) { event_received = true; },
                        [](const auto&) {},
                    },
                    event);
            });

        cmd->undo();
        EXPECT_TRUE(event_received);
    }

    TEST_F(CreateDirectoryCommandTest, DescriptionContainsDirectoryName)
    {
        auto cmd{makeCommand()};
        EXPECT_THAT(cmd->description(), testing::HasSubstr("new_folder"));
    }
};  // namespace wendfyr::tests