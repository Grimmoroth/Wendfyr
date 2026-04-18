#include "domain/commands/delete_command.hpp"

#include "wendfyr/domain/events/event.hpp"
#include "wendfyr/services/event_bus.hpp"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <filesystem>
#include <vector>

#include "gmock/gmock.h"
#include "mocks/mock_filesystem_service.hpp"

namespace wendfyr::tests
{
    class DeleteCommandTest : public ::testing::Test
    {
      protected:
        testing::NiceMock<mocks::MockFilesystemService> mock_fs;
        services::EventBus event_bus;

        // void SetUp() override
        // {
        //     ON_CALL(mock_fs, exist(testing::_)).WillByDefault(testing::Return(false));
        // }

        std::unique_ptr<domain::commands::DeleteCommand> makeSingleDeleteCommand()
        {
            std::vector<std::filesystem::path> targets{"/home/user/old_file.txt"};
            return std::make_unique<domain::commands::DeleteCommand>(targets, mock_fs, event_bus);
        }

        std::unique_ptr<domain::commands::DeleteCommand> makeMultiDeleteCommand()
        {
            std::vector<std::filesystem::path> targets{"/home/user/a.txt", "/home/user/b.txt",
                                                       "/home/user/c.txt"};

            return std::make_unique<domain::commands::DeleteCommand>(targets, mock_fs, event_bus);
        }

        void executeCommand(domain::commands::DeleteCommand& cmd)
        {
            EXPECT_CALL(mock_fs, createDirectory(testing::_)).Times(1);
            EXPECT_CALL(mock_fs, move(testing::_, testing::_)).Times(testing::AtLeast(1));

            cmd.execute();

            testing::Mock::VerifyAndClearExpectations(&mock_fs);

            ON_CALL(mock_fs, exist(testing::_)).WillByDefault(testing::Return(true));
        }
    };

    TEST_F(DeleteCommandTest, ExecuteCreatesBackupDirAndMovesFiles)
    {
        EXPECT_CALL(mock_fs, createDirectory(testing::_)).Times(1);
        EXPECT_CALL(mock_fs, move(std::filesystem::path("/home/user/old_file.txt"), testing::_))
            .Times(1);

        ON_CALL(mock_fs, exist(testing::_)).WillByDefault(testing::Return(true));
        auto cmd{makeSingleDeleteCommand()};
        cmd->execute();
    }

    TEST_F(DeleteCommandTest, ExecuteMovesMultipleFiles)
    {
        EXPECT_CALL(mock_fs, createDirectory(testing::_)).Times(1);
        EXPECT_CALL(mock_fs, move(std::filesystem::path("/home/user/a.txt"), testing::_)).Times(1);
        EXPECT_CALL(mock_fs, move(std::filesystem::path("/home/user/b.txt"), testing::_)).Times(1);
        EXPECT_CALL(mock_fs, move(std::filesystem::path("/home/user/c.txt"), testing::_)).Times(1);

        EXPECT_CALL(mock_fs, exist(testing::_)).WillRepeatedly(testing::Return(true));

        auto cmd{makeMultiDeleteCommand()};
        cmd->execute();
    }

    TEST_F(DeleteCommandTest, ExecutePublishesFilesDeletedEvent)
    {
        EXPECT_CALL(mock_fs, createDirectory(testing::_)).Times(1);
        EXPECT_CALL(mock_fs, move(testing::_, testing::_)).Times(1);
        ON_CALL(mock_fs, exist(testing::_)).WillByDefault(testing::Return(true));

        bool event_received{false};
        std::size_t deleted_count;

        event_bus.subscribe(
            [&](const domain::events::Event& event)
            {
                std::visit(
                    domain::events::Overloaded{
                        [&](const domain::events::FilesDeletedEvent& e)
                        {
                            event_received = true;
                            deleted_count = e.deleted_paths.size();
                        },
                        [](const auto&) {},
                    },
                    event);
            });

        auto cmd{makeSingleDeleteCommand()};
        cmd->execute();

        EXPECT_TRUE(event_received);
        EXPECT_EQ(deleted_count, 1);
    }

    TEST_F(DeleteCommandTest, UndoRestoresFilesFromBackup)
    {
        auto cmd{makeSingleDeleteCommand()};
        executeCommand(*cmd);

        ON_CALL(mock_fs, exist(testing::_)).WillByDefault(testing::Return(true));

        testing::Mock::VerifyAndClearExpectations(&mock_fs);
        EXPECT_CALL(mock_fs, move(testing::_, std::filesystem::path("/home/user/old_file.txt")))
            .Times(1);
        // EXPECT_CALL(mock_fs, remove(testing::_)).Times(1);

        cmd->undo();
    }

    TEST_F(DeleteCommandTest, UndoSkipsMissingBackupFiles)
    {
        auto cmd{makeMultiDeleteCommand()};
        executeCommand(*cmd);

        ON_CALL(mock_fs, exist(testing::_)).WillByDefault(testing::Return(false));

        EXPECT_CALL(mock_fs, move(testing::_, testing::_)).Times(0);

        cmd->undo();
    }

    TEST_F(DeleteCommandTest, UndoPublishesEvent)
    {
        auto cmd{makeSingleDeleteCommand()};
        executeCommand(*cmd);

        ON_CALL(mock_fs, exist(testing::_)).WillByDefault(testing::Return(true));

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

        cmd->undo();

        EXPECT_TRUE(event_received);
    }

    TEST_F(DeleteCommandTest, DestructorCleansupBackupDir)
    {
        {
            auto cmd{makeSingleDeleteCommand()};
            executeCommand(*cmd);
            ON_CALL(mock_fs, exist(testing::_)).WillByDefault(testing::Return(true));

            EXPECT_CALL(mock_fs, remove(testing::_)).Times(1);
        }
    }

    TEST_F(DeleteCommandTest, DestructorSkipsIfBackupDirAlreadyGone)
    {
        EXPECT_CALL(mock_fs, createDirectory(testing::_)).Times(1);
        EXPECT_CALL(mock_fs, move(testing::_, testing::_)).Times(1);

        {
            auto cmd{makeSingleDeleteCommand()};
            cmd->execute();

            testing::Mock::VerifyAndClearExpectations(&mock_fs);

            ON_CALL(mock_fs, exist(testing::_)).WillByDefault(testing::Return(false));
            EXPECT_CALL(mock_fs, remove(testing::_)).Times(0);
        }
    }

    TEST_F(DeleteCommandTest, DestructorWithoutExecuteDoesNothing)
    {
        {
            auto cmd{makeSingleDeleteCommand()};
        }
    }

    TEST_F(DeleteCommandTest, DescriptionSingleFile)
    {
        ON_CALL(mock_fs, exist(testing::_)).WillByDefault(testing::Return(true));

        auto cmd{makeSingleDeleteCommand()};

        EXPECT_THAT(cmd->description(), testing::HasSubstr("old_file.txt"));
    }

    TEST_F(DeleteCommandTest, DescriptionMultipleFiles)
    {
        ON_CALL(mock_fs, exist(testing::_)).WillByDefault(testing::Return(true));

        auto cmd{makeMultiDeleteCommand()};
        EXPECT_THAT(cmd->description(), testing::HasSubstr("3 files"));
    }

};  // namespace wendfyr::tests