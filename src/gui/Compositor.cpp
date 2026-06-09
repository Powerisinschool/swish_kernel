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
    Graphics::draw_rect_filled(rootSurface, Input::get_mouse_x(), Input::get_mouse_y(), 5, 5, Color{255, 0, 0, 255});

    Graphics::swap_buffers(Input::get_display_context() == DisplayContext::GUI);
}

void Compositor::inject_mouse_button(uint64_t x, uint64_t y, uint8_t button, bool is_down) const {
    const CompositorWindow *curr = tail;

    while (curr != nullptr) {
        if (const Window *win = curr->window;
            x >= win->x && x <= (win->x + win->get_surface()->width)
            && y >= win->y && y <= (win->y + win->get_surface()->height)) {
            uint64_t local_x = x - win->x;
            uint64_t local_y = y - win->y;

            win->on_mouse_button(local_x, local_y, button, is_down);

            return;
        }

        curr = curr->prev;
    }
}
