#ifndef WENDFYR_TUI_PANEL_VIEW_HPP
#define WENDFYR_TUIPANEL_VIEW_HPP
#include "wendfyr/domain/models/file_entry.hpp"
#include "wendfyr/ports/driving/i_panel_model.hpp"  // TODO: Might better to Forward declare

#include <ftxui/dom/elements.hpp>  // TODO: Same...forward declare

#include <cstddef>
#include <string>
#include <vector>

namespace wendfyr::tui
{
    class PanelView
    {
      public:
        explicit PanelView(ports::driving::IPanelModel& model);
        [[nodiscard]] ftxui::Element render(bool is_active) const;

        [[nodiscard]] int cursor() const noexcept;
        void cursorDown();
        void cursorUp();
        void cursorToTop();
        void cursorToBottom();
        void resetCursor();

        void navigateTo(const std::filesystem::path& directory);
        void navigateUp();
        void toogleSelection();
        void selectAll();
        void deselectAll();

        [[nodiscard]] std::filesystem::path currentDirectory() const;
        [[nodiscard]] std::size_t entryCount() const noexcept;
        [[nodiscard]] std::vector<domain::models::FileEntry>
        selectedEntries() const;

        [[nodiscard]] const domain::models::FileEntry* currentEntry()
            const;  // FIX: Should be <optional>

      private:
        [[nodiscard]] static std::string formatSize(std::uintmax_t bytes);
        ports::driving::IPanelModel& _model;
        int _cursor{0};
    };
};  // namespace wendfyr::tui
#endif
