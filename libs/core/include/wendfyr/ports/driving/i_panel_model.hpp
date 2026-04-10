#ifndef WENDFYR_PORTS_DRIVING_I_PANNEL_MODEL_HPP
#define WENDFYR_PORTS_DRIVING_I_PANNEL_MODEL_HPP

#include "wendfyr/domain/models/file_entry.hpp"

#include <cstddef>
#include <filesystem>
#include <span>

namespace wendfyr::ports::driving
{
    enum class SortField : std::uint8_t
    {
        NAME,
        SIZE,
        LAST_MODIFIED,
        TYPE
    };

    enum class SortOrder : std::uint8_t
    {
        ASCENDING,
        DESCENDING
    };

    class IPanelModel
    {
      public:
        virtual ~IPanelModel();
        [[nodiscard]] virtual std::filesystem::path currentDirectory() const = 0;
        [[nodiscard]] virtual std::span<const wendfyr::domain::models::FileEntry> entries()
            const = 0;
        [[nodiscard]] virtual std::size_t entryCount() const noexcept = 0;

        virtual void navigateTo(const std::filesystem::path& directory) = 0;
        virtual void navigateUp() = 0;
        virtual void refresh() = 0;

        virtual void sortBy(SortField field, SortOrder order) = 0;
        [[nodiscard]] virtual SortField currentSortField() const noexcept = 0;
        [[nodiscard]] virtual SortOrder currentSortOrder() const noexcept = 0;

        virtual void toggleSelection(std::size_t index) = 0;
        virtual void selectAll() = 0;
        virtual void deselectAll() = 0;

        [[nodiscard]] virtual std::vector<wendfyr::domain::models::FileEntry> selectedEntries()
            const = 0;
        [[nodiscard]] virtual std::size_t selectedCount() const noexcept = 0;

      protected:
        IPanelModel() = default;
        IPanelModel(const IPanelModel&) = default;
        IPanelModel(IPanelModel&&) = default;
        IPanelModel& operator=(const IPanelModel&) = default;
        IPanelModel& operator=(IPanelModel&&) = default;
    };
};  // namespace wendfyr::ports::driving

#endif