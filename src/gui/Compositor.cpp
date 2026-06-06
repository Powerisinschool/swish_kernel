#include <gui/Compositor.h>

#include "gui/Graphics.h"
#include "subsystems/input.h"

Compositor::Compositor() {
    head = nullptr;
    tail = nullptr;
}

void Compositor::add_window(Window *window) {
    auto *new_node = new CompositorWindow{nullptr, window, nullptr};

    if (head == nullptr) {
        // This is the first window on the desktop
        head = new_node;
        tail = new_node;
    } else {
        // Attach it to the top of the Z-order stack
        tail->next = new_node;
        new_node->prev = tail;
        tail = new_node;
    }
}

void Compositor::render() const {
    Graphics::draw_bg();
    const CompositorWindow *currentWindow = head;
    while (currentWindow != nullptr) {
        currentWindow->window->render();
        currentWindow = currentWindow->next;
    }
    Graphics::swap_buffers(Input::get_display_context() == DisplayContext::GUI);
}
