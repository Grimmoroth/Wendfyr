#ifndef WENDFYR_TUI_COMMAND_BAR_HPP
#define WENDFYR_TUI_COMMAND_BAR_HPP
#include <memory>

namespace ftxui
{
    class Node;
    using Element = std::shared_ptr<Node>;
};  // namespace ftxui

namespace wendfyr::tui
{

    [[nodiscard]] ftxui::Element renderCommandBar();

};

#endif
