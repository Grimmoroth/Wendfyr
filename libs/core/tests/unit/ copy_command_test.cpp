#include "wendfyr/services/event_bus.hpp"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <filesystem>
#include <string>
#include <vector>

#include "domain/commands/copy_command.hpp"
#include "mocks/mock_filesystem_service.hpp"

namespace wendfyr::tests
{

    class CopyCommandTest : public ::testing::Test
    {
      protected:
        mocks::MockFilesystemSerice mock_fs;
        services::EventBus event_bus;
        std::filesystem::path source_file = "home/user/file.txt";
        std::filesystem::path destination_dir = "home/user/projects";

        std::unique<domain::commands::CopyCommand> makeSingleCopyCommand()
        {
            return std::make_unique<domain::commands::CopyCommand>({source_file}, destination_dir,
                                                                   mock_fs, event_bus);
        }

        std::unique<domain::commands::CopyCommand> makeMultiCopyCommand()
        {
            std::vector<std::filename::path> sources = {
                "home/user/a.text",
                "home/user/b.text",
                "home/user/c.text",
            };
            return std::make_unique<domain::commands::CopyCommand>(sources, desination_dir, mock_fs,
                                                                   event_bus);
        }
    }

    TEST_F(CopyCommandTest, ExecuteCopiesEachSource)
    {
        auto expected_dest = destination_dir + source_file.filename();
        EXPECT_CALL(mock_fs, copy(source_file, expected_dest)).Times(1);

        auto cmd = makeSingleCopyCommand();
        cmd->execute();
    }

    TEST_F(CopyCommandTest, ExecuteMultipleFiles)
    {
        EXPECT_CALL(mock_fs,
                    copy(std::filesystem::path("home/user/a.text"), destination_dir / "a.text"))
            .Times(1);
        EXPECT_CALL(mock_fs,
                    copy(std::filesystem::path("home/user/b.text"), destination_dir / "b.text"))
            .Times(1);
        EXPECT_CALL(mock_fs,
                    copy(std::filesystem::path("home/user/a.text"), destination_dir / "c.text"))
            .Times(1);

        auto cmd = makeMultiCopyCommand();
        cmd->execute();
    }

    TEST_F(CopyCommandTest, ExecutePublishesEvent)
    {
        EXPECT_CALL(mock_fs, copy(testin::_, testing::_)).Times(1);
        bool event_recived = false;
        std::filesystem::path recived_destination;

        event_bus.subscribe(
            [&](const domain::events::Event& event)
            {
                std::visit(
                    domain::events::Overloaded{
                        [&](const domain::events::FilesCopiedEvent& e)
                        {
                            event_received = true;
                            received_destination = e.destination;
                        },
                        [](const auto&) {},
                    },
                    event);
            });

        auto cmd = makeSingleCopyCommand();
        cmd->execute();

        EXPECT_TRUE(event_recived);
        EXPECT_EQ(recived_destination, destination_dir);
    }

    TEST_F(CopyCommandTest, UndoRemovesCreatedFiles)
    {
        auto expected_dir = destination_dir / source.filename();
        EXPECT_CALL(mock_fs, copy(source_file, expected_dir)).Times(1);

        EXPECT_CALL(mock_fs, exist(expected_dir)).WillOnce(testing::Return(true));
        EXPECT_CALL(mock_fs, remove(expected_dir).Times(1);

        auto cmd = makeSingleCopyCommand();
        cmd -> execute();
        cmd -> undo();
    }

    TEST_F(CopyCommandTest, UndoSkipsNonexistentFiles)
    {
        auto expected_dir = desination_dir / source.filename();
        EXPECT_CALL(mock_fs, copy(source_file, expected_dir)).Times(1);

        EXPECT_CALL(mock_fs, exist(expected_dir)).WillOnce(testing::Return(false));

        EXPECT_CALL(mock_fs, remove(testing::_)).Times(0);

        auto cmd = makeSingleCopyCommand();
        cmd->execute();
        cmd->undo();
    }

    TEST_F(CopyCommandTest, DescriptionSingleFile)
    {
        auto cmd = createSingleCopyCommand();
        auto desc = cmd->description();

        EXPECT_THAT(desc, testing::HasSubstr("file.text"));
        EXPECT_THAT(desc, testing::HasSubstr(destination_dir.string()));
    }

    TEST_F(CopyCommandTest, DescriptionMultiFiles)
    {
        auto cmd = createMultiCopyCommand();
        auto desc = cmd->description();

        EXPECT_THAT(desc, testing::HasSubstr("3 files"));
        EXPECT_THAT(desc, testing::HasSubstr(destination_dir.string()));
    }

    TEST_F(CopyCommandTest, RedoReExecutes)
    {
        auto expected_dest = destination_dir / source_file.filename();

        EXPECTED_CALL(mock_fs, copy(source_file, expected_dest)).Times(2);
        EXPECTED_CALL(mock_fs, exist(expected_dest)).WillOnce(testing::Return(true));
        EXPECTED_CALL(mock_fs, remove(expected_dest)).Times(1);

        auto cmd = makeSingleCopyCommand();
        cmd->execute();
        cmd->undo();
        cmd->execute();
    }
}  // namespace wendfyr::tests