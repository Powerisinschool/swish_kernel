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

void Compositor::focus_window(const Window *window) {
    if (head == nullptr || tail == nullptr || window == nullptr) return;

    // 1. Find the CompositorWindow node for this window
    CompositorWindow *current = head;
    while (current != nullptr) {
        if (current->window == window) break;
        current = current->next;
    }

    // If the window isn't managed by us, or it is already on top, do nothing
    if (current == nullptr || current == tail) return;

    // 2. Detach 'current' from its current position
    if (current == head) {
        head = current->next;
        if (head != nullptr) head->prev = nullptr;
    } else {
        current->prev->next = current->next;
        if (current->next != nullptr) current->next->prev = current->prev;
    }

    // 3. Reattach 'current' at the tail (making it the top-most window)
    current->prev = tail;
    current->next = nullptr;
    tail->next = current;
    tail = current;
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

void Compositor::inject_mouse_button(const uint64_t x, const uint64_t y, const uint8_t button, const bool is_down) {
    const CompositorWindow *curr = tail;

    while (curr != nullptr) {
        if (Window *win = curr->window;
            x >= win->x && x <= (win->x + win->get_surface()->width)
            && y >= win->y && y <= (win->y + win->get_surface()->height)) {
            if (is_down) {
                focus_window(win);
            }

            const uint64_t local_x = x - win->x;
            const uint64_t local_y = y - win->y;

            win->on_mouse_button(local_x, local_y, button, is_down);

            return;
        }

        curr = curr->prev;
    }
}
