#include <gui/Compositor.h>

#include "drivers/mouse.h"
#include "gui/Graphics.h"
#include "subsystems/input.h"

Compositor::Compositor(Surface *surface) {
    rootSurface = surface;
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

void Compositor::inject_key(const char c) const {
    if (tail == nullptr) return;
    tail->window->inject_key(c);
}

void Compositor::render() const {
    Graphics::draw_bg(rootSurface);

    // Draw all windows
    const CompositorWindow *currentWindow = head;
    while (currentWindow != nullptr) {
        currentWindow->window->render();
        currentWindow = currentWindow->next;
    }

    // Draw the mouse cursor (a simple 5x5 red square) on top of everything
    Graphics::draw_rect_filled(rootSurface, mouse_x, mouse_y, 5, 5, Color{255, 0, 0, 255});

    Graphics::swap_buffers(Input::get_display_context() == DisplayContext::GUI);
}
