#include "command_bar.hpp"

#include <ftxui/dom/elements.hpp>

using namespace ftxui;

namespace wendfyr::tui
{

    Element renderCommandBar()
    {
        return hbox({
            text(" F5") | bold,  text(" Copy "),   separator(),
            text(" F6") | bold,  text(" Move "),   separator(),
            text(" F7") | bold,  text(" Mkdir "),  separator(),
            text(" F8") | bold,  text(" Delete "), separator(),
            text(" ^Z") | bold,  text(" Undo "),   separator(),
            text(" ^Y") | bold,  text(" Redo "),   separator(),
            text(" Tab") | bold, text(" Switch "), separator(),
            text(" Q") | bold,   text(" Quit "),   separator(),

        });
    }
}  // namespace wendfyr::tui
