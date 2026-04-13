#include "domain/panel_model.hpp"

#include "wendfyr/domain/events/event.hpp"
#include "wendfyr/domain/models/file_entry.hpp"
#include "wendfyr/ports/driving/i_panel_model.hpp"
#include "wendfyr/services/event_bus.hpp"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <filesystem>
#include <vector>

#include "mocks/mock_filesystem_service.hpp"

namespace wendfyr::tests
{
    namespace
    {
        domain::models::FileEntry makeEntry(const std::string& name, domain::models::EntryType type,
                                            std::uintmax_t size = 0)
        {
            domain::models::FileEntry entry;
            entry.path = std::filesystem::path("/test") / name;
            entry.name = name;
            entry.type = type;
            entry.selected = false;
            entry.size = size;
            return entry;
        }

        std::vector<domain::models::FileEntry> makeSampleEntries()
        {
            return {
                makeEntry("domuments", domain::models::EntryType::DIRECTORY, 0),
                makeEntry("before.txt", domain::models::EntryType::FILE, 300),
                makeEntry("about.txt", domain::models::EntryType::FILE, 100),
                makeEntry("creed.txt", domain::models::EntryType::FILE, 500),
            };
        }
    }  // namespace

    class PanelModelTest : public ::testing::Test
    {
      protected:
        mocks::MockFilesystemService mock_fs;
        services::EventBus event_bus;
        std::filesystem::path start_dir = "/test";

        std::unique_ptr<domain::PanelModel> makePanel()
        {
            EXPECT_CALL(mock_fs, listDirectory(start_dir))
                .WillOnce(testing::Return(makeSampleEntries()));
            return std::make_unique<domain::PanelModel>(mock_fs, event_bus, start_dir);
        }
    };

    TEST_F(PanelModelTest, StartsAtInitialDirectory)
    {
        auto panel = makePanel();
        EXPECT_EQ(panel->currentDirectory(), start_dir);
    }

    TEST_F(PanelModelTest, LoadEntriesConstruction)
    {
        auto panel = makePanel();
        EXPECT_EQ(panel->entryCount(), 4);
    }

    TEST_F(PanelModelTest, NavigateToChangesDirectory)
    {
        auto panel = makePanel();
        auto new_dir = std::filesystem::path("/other");
        EXPECT_CALL(mock_fs, listDirectory(new_dir))
            .WillOnce(testing::Return(std::vector<domain::models::FileEntry>{}));

        panel->navigateTo(new_dir);
        EXPECT_EQ(panel->currentDirectory(), new_dir);
    }

    TEST_F(PanelModelTest, NavigateUpGoesToParent)
    {
        auto sub_dir = std::filesystem::path("test/subdir");
        EXPECT_CALL(mock_fs, listDirectory(sub_dir))
            .WillOnce(testing::Return(std::vector<domain::models::FileEntry>{}));

        auto panel = std::make_unique<domain::PanelModel>(mock_fs, event_bus, sub_dir);
        EXPECT_CALL(mock_fs, listDirectory(std::filesystem::path("test")))
            .WillOnce(testing::Return(makeSampleEntries()));

        panel->navigateUp();
        EXPECT_EQ(panel->currentDirectory(), "/test");
    }

    TEST_F(PanelModelTest, DefaultSortByName)
    {
        auto panel = makePanel();
        auto entries = makeSampleEntries();

        EXPECT_EQ(entries[0].name, "documents");
        EXPECT_EQ(entries[1].name, "about.txt");
        EXPECT_EQ(entries[2].name, "before.txt");
        EXPECT_EQ(entries[3].name, "creed.txt");
    }

    TEST_F(PanelModelTest, SortBySize)
    {
        auto panel = makePanel();

        panel->sortBy(ports::driving::SortField::SIZE, ports::driving::SortOrder::ASCENDING);
        auto entries = makeSampleEntries();

        EXPECT_EQ(entries[0].name, "documents");
        EXPECT_EQ(entries[1].name, "after.txt");
        EXPECT_EQ(entries[2].name, "before.txt");
        EXPECT_EQ(entries[3].name, "creed.txt");
    }

    TEST_F(PanelModelTest, SortDescending)
    {
        auto panel = makePanel();
        panel->sortBy(ports::driving::SortField::SIZE, ports::driving::SortOrder::DESCENDING);

        auto entries = makeSampleEntries();
        EXPECT_EQ(entries[0].name, "creed.txt");
        EXPECT_EQ(entries[1].name, "before.txt");
        EXPECT_EQ(entries[2].name, "after.txt");
        EXPECT_EQ(entries[3].name, "documents");
    }

    TEST_F(PanelModelTest, NothingSelectedInitially)
    {
        auto panel = makePanel();
        EXPECT_EQ(panel->selectedCount(), 0);
    }

    TEST_F(PanelModelTest, ToggleSelectionSelectsEntry)
    {
        auto panel = makePanel();
        panel->toggleSelection(1);

        EXPECT_EQ(panel->selectedCount(), 1);
        auto entries = panel->selectedEntries();
        EXPECT_EQ(entries.size(), 1);
    }

    TEST_F(PanelModelTest, ToggleSelectionDeselectEntry)
    {
        auto panel = makePanel();
        panel->toggleSelection(1);
        panel->toggleSelection(1);

        EXPECT_EQ(panel->selectedCount(), 0);
    }

    TEST_F(PanelModelTest, SelectAll)
    {
        auto panel = makePanel();
        panel->selectAll();

        EXPECT_EQ(panel->selectedCount(), 4);
    }

    TEST_F(PanelModelTest, DeselectAll)
    {
        auto panel = makePanel();
        panel->selectAll();
        panel->deselectAll();

        EXPECT_EQ(panel->selectedCount(), 0);
    }

    TEST_F(PanelModelTest, ToggleOutOfBoundsIsIgnored)
    {
        auto panel = makePanel();
        panel->toggleSelection(99);

        EXPECT_EQ(panel->selectedCount(), 0);
    }

    TEST_F(PanelModelTest, NavigatePublishesDirectoryChangedEvent)
    {
        auto panel = makePanel();
        bool event_recived = false;
        std::filesystem::path recived_new_path;

        event_bus.subscribe(
            [&](const domain::events::Event& event)
            {
                std::visit(
                    domain::events::Overloaded{
                        [&](const domain::events::DirectoryChangedEvent& e)
                        {
                            event_recived = true;
                            recived_new_path = e.new_path;
                        },
                        [](const auto&) {},
                    },
                    event);
            });

        auto new_dir = std::filesystem::path("/other");
        EXPECT_CALL(mock_fs, listDirectory(new_dir))
            .WillOnce(testing::Return(std::vector<domain::models::FileEntry>{}));

        panel->navigateTo(new_dir);
        EXPECT_TRUE(event_recived);
        EXPECT_EQ(recived_new_path, new_dir);
    }

    TEST_F(PanelModelTest, RefreshesWhenFilesCopiedToCurrentDir)
    {
        auto panel = makePanel();

        EXPECT_CALL(mock_fs, listDirectory(start_dir))
            .WillOnce(testing::Return(makeSampleEntries()));

        event_bus.publish(domain::events::FilesCopiedEvent{{"/somewhere/file.txt"}, start_dir});
    }

    TEST_F(PanelModelTest, IgnoreEventsForOtherDirectories)
    {
        auto panel = makePanel();

        event_bus.publish(
            domain::events::FilesCopiedEvent{{"/somewhere/file.txt"}, "/other/directory"});
    }
};  // namespace wendfyr::tests