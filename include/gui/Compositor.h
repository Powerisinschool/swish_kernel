#pragma once
#include "Window.h"

struct CompositorWindow {
    CompositorWindow *next;
    Window *window;
    CompositorWindow *prev;
};

class Compositor {
public:
    explicit Compositor();
    void add_window(Window* window);
    void render() const;
private:
    CompositorWindow *head;
    CompositorWindow *tail;
};
