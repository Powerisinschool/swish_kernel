#pragma once
#include "Window.h"

struct CompositorWindow {
    CompositorWindow *next;
    Window *window;
    CompositorWindow *prev;
};

class Compositor {
public:
    explicit Compositor(Surface *surface);
    void add_window(Window* window);
    void focus_window(const Window* window);
    void inject_key(char c) const;
    void inject_mouse_button(uint64_t x, uint64_t y, uint8_t button, bool is_down);
    void inject_mouse_move(const uint64_t x, const uint64_t y) const;
    void render();
private:
    Surface *rootSurface;
    CompositorWindow *head;
    CompositorWindow *tail;

    void delete_window(CompositorWindow* win);
    void cleanup_closed_windows();
};
