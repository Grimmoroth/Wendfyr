#ifndef WENDFYR_DOMAIN_PANEL_MODEL_HPP
#define WENDFYR_DOMAIN_PANEL_MODEL_HPP

#include "wendfyr/domain/events/event.hpp"
#include "wendfyr/domain/models/file_entry.hpp"
#include "wendfyr/ports/driving/i_panel_model.hpp"
#include "wendfyr/services/event_bus.hpp"

#include <filesystem>
#include <vecotor>

namespace wendfyr::ports::driven
{
    class IFileSystemService;
};

namespace wendfyr::domain
{
    class PanelModel final : public ports::driving::IPanelModel
    {
      public:
        PanelModel(IFileSystemService& fs, services::EventBus event_bus,
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
        void nagivateTo(const std::filesystem::path& directory) override;
        void navigateUp() override;
        void refresh() override;

        // Sorting

        void sortBy(ports::driving::SortFiled field, ports::driving::SortOrder order override;
        void ports::driving::SortFiled currentSortField() const noexcept override;
        void ports::driving::SortOrder currentSortOrder() const noexcept override;

        // Selection
        void toggleSelection(std::size_t index) override;
        void selectAll() override;
        void deselectAll() override;
        [[nodiscard]] std::vector<domain::models::FileEntry> selectedEntries() const override;
        [[nodiscard]] std::size_t selectedCount() const noexcept override;

        private:
            void load_entries();
            void applySorting();
            void onEvent(const domain::events::Event& event);

            ports::driven::IFileSystemService& _fs;
            services::EventBus& _event_bus;
            services::SubscriptionId _subscription_id;
            std::filesystem::path _current_directory;
            std::vector<domain::models::FileEntry> _enties;

            ports::driving::SortField _sort_field{ ports::driving::SortField::NAME};
            ports::driving::SortOrder _sort_order{ ports::driving::SortOrder::ASCENDING};
    };

};  // namespace wendfyr::domain

#endif