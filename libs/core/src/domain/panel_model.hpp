#ifndef WENDFYR_DOMAIN_PANEL_MODEL_HPP
#define WENDFYR_DOMAIN_PANEL_MODEL_HPP

#include "wendfyr/domain/events/event.hpp"
#include "wendfyr/domain/models/file_entry.hpp"
#include "wendfyr/ports/driving/i_panel_model.hpp"
#include "wendfyr/services/event_bus.hpp"

#include <filesystem>
#include <vector>

namespace wendfyr::ports::driven
{
    class IFilesystemService;
};

namespace wendfyr::domain
{
    class PanelModel final : public ports::driving::IPanelModel
    {
      public:
        PanelModel(ports::driven::IFilesystemService& fs, services::EventBus& event_bus,
                   std::filesystem::path initial_directory);
        ~PanelModel() override;
        PanelModel(const PanelModel&) = delete;
        PanelModel(PanelModel&&) = delete;
        PanelModel& operator=(const PanelModel&) = delete;
        PanelModel& operator=(PanelModel&) = delete;

        // Dir State
        [[nodiscard]] std::filesystem::path currentDirectory() const override;
        [[nodiscard]] std::span<const domain::models::FileEntry> entries() const override;
        [[nodiscard]] std::size_t entryCount() const noexcept override;

        // Navigation
        void navigateTo(const std::filesystem::path& new_directory) override;
        void navigateUp() override;
        void refresh() override;

        // Sorting

        void sortBy(ports::driving::SortField field, ports::driving::SortOrder order) override;
        [[nodiscard]] ports::driving::SortField currentSortField() const noexcept override;
        [[nodiscard]] ports::driving::SortOrder currentSortOrder() const noexcept override;

        // Selection
        void toggleSelection(std::size_t index) override;
        void selectAll() override;
        void deselectAll() override;
        [[nodiscard]] std::vector<domain::models::FileEntry> selectedEntries() const override;
        [[nodiscard]] std::size_t selectedCount() const noexcept override;

      private:
        void loadEntries();
        void applySorting();
        void onEvent(const domain::events::Event& event);

        ports::driven::IFilesystemService& _fs;
        services::EventBus& _event_bus;
        services::SubscriptionId _subscription_id;
        std::filesystem::path _current_directory;
        std::vector<domain::models::FileEntry> _entries;

        ports::driving::SortField _sort_field{ports::driving::SortField::NAME};
        ports::driving::SortOrder _sort_order{ports::driving::SortOrder::ASCENDING};
    };

};  // namespace wendfyr::domain

#endif