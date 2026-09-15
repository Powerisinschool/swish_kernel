#pragma once
#include <stddef.h>
#include <stdint.h>

#include "Graphics.h"

struct Coordinate {
    size_t x, y;

    explicit Coordinate(const size_t x, const size_t y) : x(x), y(y) {}
};

class Window {
public:
    // int8_t char_index = 0;
    Coordinate loc;
    Color temp_bg;

    explicit Window(size_t x, size_t y, size_t width, size_t height, Color bg_color = Color::white);

    void inject_key(char c);
    void scroll_forward();
    void scroll_backward();

    /**
     * @brief Copy the window to the screen row by row to respect the screen's pitch
     */
    void render(bool focused) const;

    void on_mouse_button(uint64_t local_x, uint64_t local_y, uint8_t button, bool is_down);

    [[nodiscard]] const Surface *get_surface() const;
private:
    // char text_buffer[33] = {0};
    char *text_grid = nullptr;
    size_t cols, rows;
    Coordinate cursor;
    Surface *surface = nullptr;
};
