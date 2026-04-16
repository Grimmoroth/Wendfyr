#include "domain/commands/move_command.hpp"

#include "wendfyr/domain/events/event.hpp"
#include "wendfyr/services/event_bus.hpp"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "gmock/gmock.h"
#include "mocks/mock_filesystem_service.hpp"

namespace wendfyr::tests
{
    class MoveCommandTest : public ::testing::Test
    {
      protected:
        mocks::MockFilesystemService mock_fs;
        services::EventBus event_bus;

        std::filesystem::path source_file{"/home/user/file.txt"};
        std::filesystem::path dest_dir{"/home/user/archive"};

        std::unique_ptr<domain::commands::MoveCommand> makeSingleMoveCommand()
        {
            std::vector<std::filesystem::path> sources{source_file};
            return std::make_unique<domain::commands::MoveCommand>(sources, dest_dir, mock_fs,
                                                                   event_bus);
        }

        std::unique_ptr<domain::commands::MoveCommand> makeMultiMoveCommand()
        {
            std::vector sources{
                std::filesystem::path{"/home/user/a.txt"},
                std::filesystem::path{"/home/user/b.txt"},
                std::filesystem::path{"/home/user/c.txt"},
            };

            return std::make_unique<domain::commands::MoveCommand>(sources, dest_dir, mock_fs,
                                                                   event_bus);
        }
    };

    TEST_F(MoveCommandTest, ExecuteMoveEachSource)
    {
        auto expected_dir{dest_dir / source_file.filename()};

        EXPECT_CALL(mock_fs, move(source_file, expected_dir)).Times(1);

        auto cmd{makeSingleMoveCommand()};
        cmd->execute();
    }

    TEST_F(MoveCommandTest, ExecuteMovesMultipleFiles)
    {
        EXPECT_CALL(mock_fs, move(std::filesystem::path("/home/user/a.txt"), dest_dir / "a.txt"))
            .Times(1);
        EXPECT_CALL(mock_fs, move(std::filesystem::path("/home/user/b.txt"), dest_dir / "b.txt"))
            .Times(1);
        EXPECT_CALL(mock_fs, move(std::filesystem::path("/home/user/c.txt"), dest_dir / "c.txt"))
            .Times(1);

        auto cmd{makeMultiMoveCommand()};
        cmd->execute();
    }

    TEST_F(MoveCommandTest, ExecutePublishesEvents)
    {
        EXPECT_CALL(mock_fs, move(testing::_, testing::_)).Times(1);

        bool event_received{false};
        std::filesystem::path received_dest{};

        event_bus.subscribe(
            [&](const domain::events::Event& event)
            {
                std::visit(
                    domain::events::Overloaded{
                        [&](const domain::events::FilesMovedEvent& e)
                        {
                            event_received = true;
                            received_dest = e.destination;
                        },
                        [](const auto&) {},
                    },
                    event);
            });

        auto cmd{makeSingleMoveCommand()};
        cmd->execute();

        EXPECT_TRUE(event_received);
        EXPECT_EQ(received_dest, dest_dir);
    }

    TEST_F(MoveCommandTest, UndoMovesFilesBack)
    {
        auto moved_path{dest_dir / source_file.filename()};

        EXPECT_CALL(mock_fs, move(source_file, moved_path)).Times(1);
        EXPECT_CALL(mock_fs, exist(moved_path)).WillOnce(testing::Return(true));
        EXPECT_CALL(mock_fs, move(moved_path, source_file)).Times(1);

        auto cmd{makeSingleMoveCommand()};
        cmd->execute();
        cmd->undo();
    }

    TEST_F(MoveCommandTest, UndoSkipsMissingFiles)
    {
        auto moved_path{dest_dir / source_file.filename()};

        EXPECT_CALL(mock_fs, move(source_file, moved_path)).Times(1);
        EXPECT_CALL(mock_fs, exist(moved_path)).WillOnce(testing::Return(false));
        EXPECT_CALL(mock_fs, move(moved_path, source_file)).Times(0);

        auto cmd{makeSingleMoveCommand()};
        cmd->execute();
        cmd->undo();
    }

    TEST_F(MoveCommandTest, UndoMultipleFilesInReverseOrder)
    {
        EXPECT_CALL(mock_fs, move(testing::_, testing::_)).Times(3);
        EXPECT_CALL(mock_fs, exist(testing::_)).WillRepeatedly(testing::Return(true));

        auto cmd{makeMultiMoveCommand()};
        cmd->execute();

        testing::Mock::VerifyAndClearExpectations(&mock_fs);

        EXPECT_CALL(mock_fs, exist(testing::_)).WillRepeatedly(testing::Return(true));
        EXPECT_CALL(mock_fs, move(testing::_, testing::_)).Times(3);

        cmd->undo();
    }

    TEST_F(MoveCommandTest, DescriptionSingleFile)
    {
        auto cmd{makeSingleMoveCommand()};
        auto desc{cmd->description()};

        EXPECT_THAT(desc, testing::HasSubstr("file.txt"));
        EXPECT_THAT(desc, testing::HasSubstr(dest_dir.string()));
    }

    TEST_F(MoveCommandTest, DescriptionMultipleFiles)
    {
        auto cmd{makeMultiMoveCommand()};
        auto desc{cmd->description()};

        EXPECT_THAT(desc, testing::HasSubstr("3 files"));
    }

    TEST_F(MoveCommandTest, RedoReExecutes)
    {
        auto moved_path{dest_dir / source_file.filename()};

        EXPECT_CALL(mock_fs, move(source_file, moved_path)).Times(2);

        EXPECT_CALL(mock_fs, exist(moved_path)).WillOnce(testing::Return(true));
        EXPECT_CALL(mock_fs, move(moved_path, source_file)).Times(1);

        auto cmd{makeSingleMoveCommand()};
        cmd->execute();
        cmd->undo();
        cmd->execute();
    }
}  // namespace wendfyr::tests