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
    void inject_key(char c) const;
    void render() const;
private:
    Surface *rootSurface;
    CompositorWindow *head;
    CompositorWindow *tail;
};
